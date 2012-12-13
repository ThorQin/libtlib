key:		auto|register|static|extern|volatile
key:		char|wchar_t|short|int|long|float|double|void|signed|unsigned|struct|enum|union
key:		for|do|while|goto|continue|break|switch|case|default|if|else|return
key:		new|delete|try|catch|throw|using|namespace|asm|mutable|const|bool|true|false
key:		typedef|operator|inline
key:		class|explicit|private|protected|public|friend|this|virtual
key:		template|typename|typeid|type_info
key:		bad_cast|bad_typeid|const_cast|dynamic_cast|reinterpret_cast|static_cast
id:			[a-zA-Z_][a-zA-Z0-9_]*
number:		0x[0-9a-fA-F]+L?
number:		[0-9]+
number:		[0-9]*\.[0-9]+
string:		"([^\"\n]|\\")*"
string:		'([^\'\n]|\\')*'
comment(m):	/\*.*\*/
comment: 	//[^\n]*
process: 	#include|#if|#else|#elif|#endif|#ifdef|#define|#undef
operator: 	\(|\)|{|}|\[|\]|<|>|!|\?|\.|,|;|:|::|\+|-|\*|/|%|\^|\||&|~|=|\+=|-=|\*=|/=|%=|^=|~=|\|=|&=|==|!=|\|\||&&


