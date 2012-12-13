#ifndef __QN_BPT_H__
#define __QN_BPT_H__

#ifdef _MSC_VER
#	include <io.h>
#else
#	include <sys/io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <algorithm>
#ifdef __MSVC__
#include <share.h>
#include <Windows.h>
#else
#include <sys/file.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#ifdef _MSC_VER
inline FILE* _open_file_for_write(const char* filename) {
	return _fsopen(filename, "r+b", _SH_DENYRW);
}
inline FILE* _open_file_for_read(const char* filename) {
	return _fsopen(filename, "rb", _SH_DENYWR);
}
inline FILE* _create_file_for_write(const char* filename) {
	return _fsopen(filename, "w+b", _SH_DENYWR);
}
inline int _seek_file_pos(FILE* file, long long pos, int flag) {
	return _fseeki64(file, pos, flag);
}
inline long long _tell_file_pos(FILE* file) {
	return (long long)_ftelli64(file);
}
#else
inline FILE* _open_file_for_write(const char* filename) {
	FILE* file = fopen64(filename, "r+b");
	if (file)
		flock(fileno(file), LOCK_SH);
	return file;
}
inline FILE* _open_file_for_read(const char* filename) {
	FILE* file = fopen64(filename, "rb");
	if (file)
		flock(fileno(file), LOCK_SH);
	return file;
}
inline FILE* _create_file_for_write(const char* filename) {
	FILE* file = fopen64(filename, "w+b");
	if (file)
		flock(fileno(file), LOCK_SH);
	return file;
}
inline int _seek_file_pos(FILE* file, long long pos, int flag) {
	return fseeko64(file, pos, flag);
}
inline long long _tell_file_pos(FILE* file) {
	return ftello64(file);
}
#endif

template<const unsigned _PageSize = 4096>
class BPTree {
public:
	static const unsigned pageSize = _PageSize > 64 ? _PageSize : 64;
	typedef unsigned long long KeyType;
	typedef unsigned long long ValueType;
	static const KeyType key_min = 0;
	static const KeyType key_max = 0xFFFFFFFFFFFFFFFF;
	typedef enum Option {
		more_or_equal = 0, less_or_equal = 1
	} Option;

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#endif
	typedef struct NodePair {
		KeyType key;
		ValueType value;
	}
#ifdef __GNUC__
	__attribute__((packed))
#endif
	NodePair;
private:
	static const unsigned node_pair_size = sizeof(NodePair);
	static const unsigned node_items = ((pageSize - sizeof(unsigned))
			/ node_pair_size) - 1;
	typedef struct Node {
		unsigned count;
		NodePair pairs[node_items];
	}
#ifdef __GNUC__
	__attribute__((packed))
#endif
	Node;

	typedef struct Info {
		char mark[8];
		unsigned long long count;
		unsigned long long height;
		unsigned long long rootOffset;
		unsigned long long fileSize;
	}
#ifdef __GNUC__
	__attribute__((packed))
#endif
	Info;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

	typedef struct Cursor {
		inline Cursor() :
				offset(0), pos(-1), changed(true) {
			memset(page, 0, pageSize);
		}
		unsigned char page[pageSize];
		unsigned long long offset;
		int pos;
		bool changed;
	} Cursor;
private:
	int internal_search(NodePair* pairs, unsigned count, KeyType key,
			Option option);
	void internal_search_insert(KeyType key);
	void internal_open(const char* filename, bool readonly);
	void internal_create(const char* filename);
	void internal_load();
	bool internal_load_next_page(int level);
	bool internal_load_prev_page(int level);
	inline Cursor* internal_load_cursor(unsigned long long offset);
	inline void internal_save_cursor(Cursor* cursor);
	void internal_insert(int level, KeyType key, ValueType value);
	void internal_split(int level);
public:
	BPTree(const char* filename, bool readonly = false);
	void flush();
	const NodePair* search(KeyType key, Option option = less_or_equal);
	void insert(KeyType key, ValueType value);
	void remove(KeyType key);

	inline const NodePair* current();
	inline const NodePair* first();
	inline const NodePair* next();
	inline const NodePair* prev();
	inline const NodePair* last();

	size_t set_ext_data(const unsigned char* data, size_t len);
	size_t get_ext_data(unsigned char* data, size_t len);

	inline const unsigned long long count() const;
	~BPTree();
private:
	FILE* file;
	Info* info;
	std::vector<Cursor*> cursors;
	bool readonly;
	bool changed;
};

static const char* bpt_mark = "QnBPT.01";

template<const unsigned int pageSize>
int BPTree<pageSize>::internal_search(NodePair* pairs, unsigned count,
		KeyType key, Option option) {
	if (count == 0)
		return -1;
	int l = 0, r = (int) count - 1, m;
	while (l <= r) {
		m = (l + r) >> 1;
		if (pairs[m].key == key)
			goto FIND;
		else if (pairs[m].key < key)
			l = m + 1;
		else
			r = m - 1;
	}
	return (option == less_or_equal) ?
			(l - 1 < 0 ? 0 : l - 1) :
			(r + 1 >= (int) count ? (int) count - 1 : r + 1);
	FIND: if (option == more_or_equal) {
		while (m > 0) {
			if (pairs[m - 1].key == pairs[m].key)
				m--;
			else
				break;
		}
	} else // if (option == less_or_equal)
	{
		while (m + 1 < (int) count) {
			if (pairs[m + 1].key == pairs[m].key)
				m++;
			else
				break;
		}
	}
	return m;
}

template<const unsigned int pageSize>
inline const unsigned long long BPTree<pageSize>::count() const {
	return info->count;
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_create(const char* filename) {
	file = _create_file_for_write(filename);
	if (!file)
		throw std::runtime_error("open file error.");
	memcpy(info->mark, bpt_mark, 8);
	info->count = 0;
	info->height = 1;
	info->rootOffset = pageSize;
	info->fileSize = pageSize + pageSize;
	changed = true;
	readonly = false;
	cursors.push_back(new Cursor());
	cursors[0]->offset = pageSize;
}

template<const unsigned int pageSize>
inline typename BPTree<pageSize>::Cursor* BPTree<pageSize>::internal_load_cursor(
		unsigned long long offset) {
	if (offset % pageSize != 0)
		throw std::runtime_error("file error.");
	_seek_file_pos(file, offset, SEEK_SET);

	Cursor* cursor = new Cursor();
	size_t readSize = fread(cursor->page, 1, pageSize, file);
	if (readSize == 0) {
		delete cursor;
		throw std::runtime_error("file error.");
	}
	cursor->offset = offset;
	cursor->pos = -1;
	cursor->changed = false;
	return cursor;
}

template<const unsigned int pageSize>
inline void BPTree<pageSize>::internal_save_cursor(Cursor* cursor) {
	if (cursor->changed) {
		_seek_file_pos(file, cursor->offset, SEEK_SET);
		fwrite(cursor->page, 1, pageSize, file);
		cursor->changed = false;
	}
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_load() {
	_seek_file_pos(file, 0, SEEK_SET);
	size_t readSize = fread(info, 1, pageSize, file);
	if (readSize == 0)
		throw std::runtime_error("file error.");

	if (readSize < pageSize)
		throw std::runtime_error("file invalid.");

	if (memcmp(info->mark, bpt_mark, 8) != 0)
		throw std::runtime_error("file invalid.");

	_seek_file_pos(file, 0, SEEK_END);
	unsigned long long filesize = _tell_file_pos(file);

	if (filesize != info->fileSize)
		throw std::runtime_error("file invalid.");
	cursors.push_back(internal_load_cursor(info->rootOffset));
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_open(const char* filename, bool readonly) {
	if (readonly) {
		if (!(file = _open_file_for_read(filename)))
			throw std::runtime_error("open file error.");
	} else if (!(file = _open_file_for_write(filename))) {
		internal_create(filename);
		return;
	}
	internal_load();
	this->readonly = readonly;
	changed = false;
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_insert(int level, KeyType key,
		ValueType value) {
	Cursor* cursor = cursors[level];
	Node* node = (Node*) cursor->page;
	if (cursor->pos < 0) // Node is empty so insert directly
			{
		cursor->pos = 0;
		node->pairs[0].key = key;
		node->pairs[0].value = value;
		node->count++;
		cursor->changed = true;
		if (level == info->height - 1) {
			info->count++;
			changed = true;
		}
	} else {
		if (key >= node->pairs[cursor->pos].key)
			cursor->pos++;
		memcpy(&node->pairs[cursor->pos + 1], &node->pairs[cursor->pos],
				(size_t) (node->count - cursor->pos) * sizeof(NodePair));
		node->pairs[cursor->pos].key = key;
		node->pairs[cursor->pos].value = value;
		node->count++;
		cursor->changed = true;
		if (level == info->height - 1) {
			info->count++;
			changed = true;
		}
		if (node->count > node_items)
			internal_split(level);
	}
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_split(int level) {
	changed = true;
	Cursor* cursor = cursors[level];
	Node* node = (Node*) cursor->page;

	Cursor* newCursor = new Cursor();
	Node* newNode = (Node*) newCursor->page;

	newCursor->offset = info->fileSize;
	info->fileSize += pageSize;
	newCursor->changed = true;

	int pos = cursor->pos;
	int movePos = node->count / 2;

	memcpy(newNode->pairs, &node->pairs[movePos],
			(size_t) (node->count - movePos) * sizeof(NodePair));
	memset(&node->pairs[movePos], 0,
			(size_t) (node->count - movePos) * sizeof(NodePair));

	newNode->count = node->count - movePos;
	node->count = movePos;

	KeyType newKey = newNode->pairs[0].key;
	ValueType newOffset = newCursor->offset;
	ValueType oldOffset = cursor->offset;

	if (pos >= movePos) // pos in new cursor
			{
		internal_save_cursor(cursor);
		newCursor->pos = pos - movePos;
		delete cursors[level];
		cursors[level] = newCursor;
	} else // pos in old cursor
	{
		newCursor->changed = true;
		internal_save_cursor(newCursor);
		delete newCursor;
		cursor->changed = true;
	}

	if (level == 0) // is root node
			{
		cursors.insert(cursors.begin(), new Cursor());
		Cursor* root = cursors[0];
		Node* rootNode = (Node*) root->page;
		root->offset = info->fileSize;
		info->fileSize += pageSize;
		info->rootOffset = root->offset;
		info->height++;
		rootNode->pairs[0].key = key_min;
		rootNode->pairs[0].value = oldOffset;
		rootNode->pairs[1].key = newKey;
		rootNode->pairs[1].value = newOffset;
		rootNode->count = 2;
		root->pos = pos >= movePos ? 1 : 0;
		root->changed = true;
	} else {
		internal_insert(level - 1, newKey, newOffset);
	}
}

template<const unsigned int pageSize>
BPTree<pageSize>::BPTree(const char* filename, bool readonly) {
	cursors.reserve(32);
	info = (Info*) malloc(pageSize);
	memset(info, 0, pageSize);
	file = NULL;
	try {
		internal_open(filename, readonly);
	} catch (...) {
		if (file)
			fclose(file);
		free(info);
		throw;
	}
}

template<const unsigned int pageSize>
void BPTree<pageSize>::flush() {
	if (readonly || !changed)
		return;
	_seek_file_pos(file, 0, SEEK_SET);
	fwrite(info, 1, pageSize, file);
	for (size_t i = 0; i < cursors.size(); i++) {
		internal_save_cursor(cursors[i]);
	}
	changed = false;
}

template<const unsigned int pageSize>
void BPTree<pageSize>::internal_search_insert(KeyType key) {
	int i = 0;
	unsigned long long offset = info->rootOffset;
	do {
		// Cache not load, so load it
		if (i + 1 > (int) cursors.size()) {
			cursors.push_back(internal_load_cursor(offset));
		}
		if (cursors[i]->offset != offset) {
			flush();
			delete cursors[i];
			cursors[i] = internal_load_cursor(offset);
		}
		Cursor* cursor = cursors[i];
		Node* node = (Node*) cursor->page;
		cursor->pos = internal_search(node->pairs, node->count, key,
				less_or_equal);
		if (i != info->height - 1) // is Node
				{
			if (cursor->pos == -1)
				throw std::runtime_error("invalid file");
			offset = node->pairs[cursor->pos].value;
		}
	} while (++i < info->height);
}

template<const unsigned int pageSize>
inline const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::current() {
	unsigned long long offset = info->rootOffset;
	for (int i = 0; i < info->height; i++) {
		if (i + 1 > (int) cursors.size())
			return NULL;
		Cursor* cursor = cursors[i];
		if (cursor->offset != offset)
			return NULL;
		if (cursor->pos == -1)
			return NULL;
		Node* node = (Node*) cursor->page;
		if (i == info->height - 1) // is Leaf
			return &node->pairs[cursor->pos];
		else
			offset = node->pairs[cursor->pos].value;
	}
	return NULL;
}

template<const unsigned int pageSize>
const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::search(KeyType key,
		Option option) {
	internal_search_insert(key);
	const NodePair* pair = current();
	if (!pair || option == less_or_equal)
		return pair;

	if (pair->key != key) {
		if (pair->key > key)
			return pair;
		else
			return next();
	} else {
		while (pair && pair->key == key)
			pair = prev();
		if (pair == NULL)
			return current();
		else
			return next();
	}
}

template<const unsigned int pageSize>
void BPTree<pageSize>::insert(KeyType key, ValueType value) {
	if (readonly)
		throw std::runtime_error("invalid operation in readonly mode");
	internal_search_insert(key);
	if (cursors.size() != info->height)
		throw std::runtime_error("invalid file");
	internal_insert((int) info->height - 1, key, value);
}

template<const unsigned int pageSize>
void BPTree<pageSize>::remove(KeyType key) {
	if (readonly)
		throw std::runtime_error("invalid operation in readonly mode");
	throw std::runtime_error("Need be implemented.");
}

template<const unsigned int pageSize>
inline const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::first() {
	return search(key_min, more_or_equal);
}

template<const unsigned int pageSize>
bool BPTree<pageSize>::internal_load_next_page(int level) {
	if (level <= 0)
		return false;
	Cursor* cursor = cursors[level - 1];
	Node* node = (Node*) cursor->page;
	if (cursor->pos + 1 < (int) node->count) {
		++cursor->pos;
		flush();
		delete cursors[level];
		cursors[level] = internal_load_cursor(node->pairs[cursor->pos].value);
		cursors[level]->pos = 0;
		return true;
	} else {
		if (internal_load_next_page(level - 1)) {
			cursor = cursors[level - 1];
			node = (Node*) cursor->page;
			flush();
			delete cursors[level];
			cursors[level] = internal_load_cursor(
					node->pairs[cursor->pos].value);
			cursors[level]->pos = 0;
			return true;
		} else
			return false;
	}
}

template<const unsigned int pageSize>
inline const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::next() {
	if (cursors.size() < info->height)
		return NULL;
	Cursor* cursor = cursors[(unsigned) info->height - 1];
	Node* node = (Node*) cursor->page;
	if (cursor->pos + 1 < (int) node->count) {
		return &node->pairs[++cursor->pos];
	} else {
		if (internal_load_next_page((unsigned) info->height - 1)) {
			cursor = cursors[(unsigned) info->height - 1];
			node = (Node*) cursor->page;
			return &node->pairs[cursor->pos];
		} else {
			return NULL;
		}
	}
}

template<const unsigned int pageSize>
bool BPTree<pageSize>::internal_load_prev_page(int level) {
	if (level <= 0)
		return false;
	Cursor* cursor = cursors[level - 1];
	Node* node = (Node*) cursor->page;
	if (cursor->pos > 0) {
		--cursor->pos;
		flush();
		delete cursors[level];
		cursors[level] = internal_load_cursor(node->pairs[cursor->pos].value);
		cursors[level]->pos = ((Node*) cursors[level]->page)->count - 1;
		return true;
	} else {
		if (internal_load_prev_page(level - 1)) {
			cursor = cursors[level - 1];
			node = (Node*) cursor->page;
			flush();
			delete cursors[level];
			cursors[level] = internal_load_cursor(
					node->pairs[cursor->pos].value);
			cursors[level]->pos = ((Node*) cursors[level]->page)->count - 1;
			return true;
		} else
			return false;
	}
}

template<const unsigned int pageSize>
inline const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::prev() {
	if (cursors.size() < info->height)
		return NULL;
	Cursor* cursor = cursors[(unsigned) info->height - 1];
	Node* node = (Node*) cursor->page;
	if (cursor->pos > 0) {
		return &node->pairs[--cursor->pos];
	} else {
		if (internal_load_prev_page((unsigned) info->height - 1)) {
			cursor = cursors[(unsigned) info->height - 1];
			node = (Node*) cursor->page;
			return &node->pairs[cursor->pos];
		} else
			return NULL;
	}
}

template<const unsigned int pageSize>
inline const typename BPTree<pageSize>::NodePair* BPTree<pageSize>::last() {
	return search(key_max, less_or_equal);
}

template<const unsigned int pageSize>
BPTree<pageSize>::~BPTree() {
	flush();
	for (size_t i = 0; i < cursors.size(); i++)
		delete cursors[i];
	fclose(file);
	free(info);
}

template<const unsigned int pageSize>
size_t BPTree<pageSize>::set_ext_data(const unsigned char* data, size_t len) {
	if (readonly)
		throw std::runtime_error("invalid operation in readonly mode");
	unsigned char* store = ((unsigned char*) info) + sizeof(Info);
	size_t storeSize = pageSize - sizeof(Info);
	size_t copySize = len > storeSize ? storeSize : len;
	memcpy(store, data, copySize);
	changed = true;
	return copySize;
}

template<const unsigned int pageSize>
size_t BPTree<pageSize>::get_ext_data(unsigned char* data, size_t len) {
	unsigned char* store = ((unsigned char*) info) + sizeof(Info);
	size_t storeSize = pageSize - sizeof(Info);
	size_t copySize = len > storeSize ? storeSize : len;
	memcpy(data, store, copySize);
	return copySize;
}

#endif // __QN_BP_TREE__
