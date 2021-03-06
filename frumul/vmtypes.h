#ifndef VMTYPES_H
#define VMTYPES_H
/* This file defines the types used in the Virtual Machine
 */
#include <cassert>
#include "valvar.h"
#include <map>
#include <memory>
#include "exception.h"
#include "macros.h"
#include "node.h"



#define EXPR_TYPES(T)\
	T(MIN_PRIMITIVE)\
	T(VOID)\
	\
	T(INT)\
	T(TEXT)\
	T(BOOL)\
	T(MAX_PRIMITIVE)\
	\
	/*containers */\
	T(MIN_CONTAINER)\
	T(SYMBOL)\
	T(LIST)\
	T(MAX_CONTAINER)\
	\
	/* modifiers */\
	T(CONSTANT)\
	T(STATIC)\
	T(VARIABLE)\
        T(UNSAFE_SYMBOL)\
	T(STACK_ELT)\
        T(MAX_MODIFIERS)\
        \
        /* generics */ \
        T(INDEXABLE)\
        T(CALLABLE)\
        T(TAILABLE)\
        T(MAX_GENERICS)\
        \

namespace frumul {

	class ExprType {
		/* This class defines the types used in the compiler
		 */
		public:
			ENUM(Type,EXPR_TYPES)
			// constructors

			ExprType(ExprType::Type);
			ExprType(const ExprType&);
			ExprType(const Node&);
			ExprType(ExprType::Type, const ExprType&);
			ExprType(ExprType::Type container, std::unique_ptr<ExprType> ncontained);
			// functions
			// assignment
			ExprType& operator = (ExprType::Type);
			ExprType& operator = (const ExprType&);
			// setters
			ExprType& setContained(ExprType::Type);
			// bools
			bool operator == (const ExprType&)const;
			bool operator == (const ExprType::Type)const;
			bool operator != (const ExprType&)const;
			bool operator != (const ExprType::Type)const;
			bool operator & (const ExprType::Type)const;
			bool isContainer () const;
			bool isConst () const;
			bool isStatic() const;
			bool check(const ValVar&) const;
                        bool isGeneric(const ExprType::Type) const;
                        bool isGeneric() const;
                        bool isUnsafe() const;
			// getters
			const ExprType& getContained() const;
			ExprType& getContained() ;
			Type getPrimitive () const;
			Type getPrimitive(Type) const;
			Type getType() const;
			int getDepth() const;
			int getDepth(Type) const;
			// cast
			operator Type () const;

			// display
			FString toString(bool asChild = false) const;
			STDOUT(ExprType)
		private:
			Type type;
			bool is_const{false};
			bool is_static{false};
			std::unique_ptr<ExprType> contained;
			const static std::map<FString,ExprType::Type> type_names;

			// functions
			void checkNode(const NodeVector& types, const NodeVector::const_reverse_iterator it);

	};
	using ET = ExprType;
}
#endif
