/************************************************************************
*
*  LibTLib
*  Copyright (C) 2010  Thor Qin
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* Author: Thor Qin
* Bug Report: thor.qin@gmail.com
*
**************************************************************************/

#include "../../include/tlib/lex/dfa.h"
#include <map>
#include <set>
#include <vector>

namespace tlib
{
namespace lex
{

using namespace std;

template <class T>
struct RangeInfo
{
	Range<T> range;
	Action action;
};


template <class T>
struct ComputeInfo
{
	typedef vector<RangeInfo<T> > PosRangeArray;
	typedef vector<unsigned int> PosArray;
	typedef vector<PosArray> FollowArray;
	PosRangeArray pos_ranges;
	FollowArray follows;
	unsigned int id; // A counter use to identify nodes
};

template <class T>
void compute_node(typename Exp<T>::ExpPtr exp, void *param)
{
	ComputeInfo<T> *context = static_cast<ComputeInfo<T>*> (param);

	if (exp->type == RE_NODE_OR)
	{
		exp->nullable = exp->exp.left->nullable
				|| exp->exp.right->nullable;
		exp->firstpos.insert(exp->firstpos.end(),
				exp->exp.left->firstpos.begin(),
				exp->exp.left->firstpos.end());
		exp->firstpos.insert(exp->firstpos.end(),
				exp->exp.right->firstpos.begin(),
				exp->exp.right->firstpos.end());
		exp->lastpos.insert(exp->lastpos.end(),
				exp->exp.left->lastpos.begin(),
				exp->exp.left->lastpos.end());
		exp->lastpos.insert(exp->lastpos.end(),
				exp->exp.right->lastpos.begin(),
				exp->exp.right->lastpos.end());
		if (exp->action != 0)
		{
			for (unsigned int i = 0; i < exp->lastpos.size(); ++i)
			{
				context->pos_ranges[exp->lastpos[i]].action = exp->action;
			}
		}
	}
	else if (exp->type == RE_NODE_AND)
	{
		exp->nullable = exp->exp.left->nullable
				&& exp->exp.right->nullable;
		exp->firstpos.insert(exp->firstpos.end(),
				exp->exp.left->firstpos.begin(),
				exp->exp.left->firstpos.end());
		if (exp->exp.left->nullable)
			exp->firstpos.insert(exp->firstpos.end(),
					exp->exp.right->firstpos.begin(),
					exp->exp.right->firstpos.end());
		if (exp->exp.right->nullable)
			exp->lastpos.insert(exp->lastpos.end(),
					exp->exp.left->lastpos.begin(),
					exp->exp.left->lastpos.end());
		exp->lastpos.insert(exp->lastpos.end(),
				exp->exp.right->lastpos.begin(),
				exp->exp.right->lastpos.end());
		if (exp->action != 0)
		{
			for (unsigned int i = 0; i < exp->lastpos.size(); ++i)
			{
				context->pos_ranges[exp->lastpos[i]].action = exp->action;
			}
		}
		// Compute follow pos for AND operator.
		vector<unsigned int>::iterator it = exp->exp.left->lastpos.begin();
		while (it != exp->exp.left->lastpos.end())
		{
			if (context->follows.size() < *it + 1)
			{
				context->follows.resize(*it + 1);
			}
			context->follows[*it].insert(context->follows[*it].end(),
					exp->exp.right->firstpos.begin(),
					exp->exp.right->firstpos.end());
			++it;
		}
	}
	else if (exp->type == RE_NODE_RANGE)
	{
		exp->nullable = false;
		exp->firstpos.push_back(context->id);
		exp->lastpos.push_back(context->id);
		if (context->pos_ranges.size() < context->id + 1)
		{
			context->pos_ranges.resize(context->id + 1);
			context->follows.resize(context->id + 1);
		}
		context->pos_ranges[context->id].range = exp->range;
		context->pos_ranges[context->id].action = exp->action;
		context->id++;
	}
	else
	{
		if (exp->type == RE_NODE_QUESTION || exp->type == RE_NODE_CLOSURE)
			exp->nullable = true;
		else if (exp->type == RE_NODE_PLUS)
			exp->nullable = false;
		exp->firstpos.insert(exp->firstpos.end(),
				exp->child->firstpos.begin(),
				exp->child->firstpos.end());
		exp->lastpos.insert(exp->lastpos.end(),
				exp->child->lastpos.begin(),
				exp->child->lastpos.end());
		if (exp->action != 0)
		{
			for (unsigned int i = 0; i < exp->lastpos.size(); ++i)
			{
				context->pos_ranges[exp->lastpos[i]].action = exp->action;
			}
		}
		if (exp->type == RE_NODE_CLOSURE || exp->type == RE_NODE_PLUS)
		{
			vector<unsigned int>::iterator it = exp->lastpos.begin();
			while (it != exp->lastpos.end())
			{
				if (context->follows.size() < *it + 1)
				{
					context->follows.resize(*it + 1);
				}
				context->follows[*it].insert(context->follows[*it].end(),
						exp->firstpos.begin(), 
						exp->firstpos.end());
				++it;
			}
		}
	}
}

/* Compute the expression's node values.  */
template <class T>
void make_dfa(typename Exp<T>::ExpPtr exp, Dfa<T>& dfa)
{
	typedef typename ComputeInfo<T>::PosArray PosArray;
	typedef map<PosArray, unsigned int> StateMap;
	ComputeInfo<T> context;
	// Initialize counter.
	context.id = 0;
	// First, extend the expression with <EOF> mark.
	Range<T> range = Range<T>::empty_range;
	typename Exp<T>::ExpPtr root(
		new Exp<T>(RE_NODE_AND, exp, typename Exp<T>::ExpPtr(new Exp<T>(range))));
	// Compute nullable, firstpos, lastpos and followpos array.
	exp_depth_travel<T>(root, compute_node<T>, &context);

	// An id of the node that a range node of <EOF>
	unsigned int finished_id = 0;
	for (unsigned int i = 0; i < context.pos_ranges.size(); i++)
	{
		if (context.follows[i].size() == 0)
		{
			finished_id = i;
			break;
		}
	}

	unsigned int state_id = 0;
	StateMap marked_map, unmark_map;
	unmark_map[root->firstpos] = state_id++;

	// Get all of the input except the last ZERO symbol.
	typename Dfa<T>::RangeVector ranges;
	for (unsigned int i = 0; i < context.pos_ranges.size() - 1; i++)
	{
		ranges.push_back(context.pos_ranges[i].range);
	}

	// Thor Qin range split algorithm. :-)
	split_range<T>(ranges);

	dfa.range_map.reserve(ranges.size());
	dfa.range_map.insert(dfa.range_map.begin(), ranges.begin(), ranges.end());

	while (!unmark_map.empty())
	{
		// Add to marked collection.
		PosArray pos_array = unmark_map.begin()->first;
		unsigned int id = unmark_map.begin()->second;
		marked_map[pos_array] = id;
		unmark_map.erase(unmark_map.begin());

		for (unsigned int i = 0; i < ranges.size(); i++)
		{
			// Check every non-intersect ranges.
			Action range_action = 0;
			PosArray to_state;
			set<unsigned int> to_set;
			for (unsigned int j = 0; j < pos_array.size(); j++)
			{
				if (relation<T>(context.pos_ranges[pos_array[j]].range, ranges[i])
						!= R_UNRELATED)
				{
					// Sometimes the definition of regular expressions will be
					// made into a conflict, such as "IF", it both can be used as
					// a keyword or an ID symbol, So, should us return the "keyword IF" or
					// the "variable name IF"?
					// Here actually has two strategies: Either is the first definition has
					// a higher priority, or the later definition cover the before.
					// For use strategy 1, add:
					//		"range_action == 0"
					// For use strategy 2:
					//		"context.pos_ranges[pos_array[j]].action != 0"
					// Choose between the two.
					// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					if (range_action == 0)
					// if (context.pos_ranges[pos_array[j]].action != 0)
						range_action = context.pos_ranges[pos_array[j]].action;
					// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					to_set.insert(context.follows[pos_array[j]].begin(),
							context.follows[pos_array[j]].end());
				}
			}
			to_state.insert(to_state.end(), to_set.begin(), to_set.end());
			if (!to_state.empty())
			{
				if (marked_map.find(to_state) != marked_map.end())
				{
					Transit t =
					{id, marked_map[to_state], i, range_action};
					dfa.transits.push_back(t);
				}
				else if (unmark_map.find(to_state) != unmark_map.end())
				{
					Transit t =
					{id, unmark_map[to_state], i, range_action};
					dfa.transits.push_back(t);
				}
				else
				{
					unmark_map[to_state] = state_id++;
					Transit t =
					{id, unmark_map[to_state], i, range_action};
					dfa.transits.push_back(t);
				}
			}
		}
	}

	dfa.total_states = (unsigned int)marked_map.size();
	// So, in the end, "marked_map" have all of the states.
	for (typename StateMap::iterator it = marked_map.begin();
			it != marked_map.end(); ++it)
	{
		// Record the group that have the final state.
		for (unsigned int i = 0; i < it->first.size(); ++i)
		{
			if (it->first[i] == finished_id)
			{
				dfa.accepting_states.push_back(it->second);
				break;
			}
		}
	}
}

template void make_dfa<char>(Exp<char>::ExpPtr exp, Dfa<char>& dfa);
template void make_dfa<unsigned char>(Exp<unsigned char>::ExpPtr exp, Dfa<unsigned char>& dfa);
template void make_dfa<wchar_t>(Exp<wchar_t>::ExpPtr exp, Dfa<wchar_t>& dfa);


} // End of namespace lex
} // End of namespace tlib
