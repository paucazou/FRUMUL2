#include "vmtypes.h"

namespace frumul {

	const std::map<bst::str,ExprType::Type> ExprType::type_names {
                                {"void",ET::VOID},
                                {"int",ET::INT},
                                {"text",ET::TEXT},
                                {"bool",ET::BOOL},
                                {"symbol",ET::SYMBOL},
                                {"list",ET::LIST},
				{"const",ET::CONSTANT},
				{"static",ET::STATIC},
				};

	ExprType::ExprType( ExprType::Type t) : type{t}
	{
		/* Constructs a simple class with
		 * no contained
		 */
	}

	ExprType::ExprType(const ExprType& other) :
		type{other.type}
	{
		if (other.contained)
			contained = std::make_unique<ExprType>(*other.contained);
	}

	ExprType::ExprType (const Node& n) {
		/* Compile a type from a node
		 */
		assert(n.type() == Node::RETURN_TYPE &&"Node is not a type");

		const auto& types{n.getNumberedChildren()};

		// first type
		checkNode(types,types.crbegin());
		ExprType t{type_names.at(types.crbegin()->getValue())};

		for (auto it {types.crbegin() + 1}; it != types.crend(); ++it) {
			checkNode(types,it);
			if (it->type() == Node::LITINT) {
				int nb{ static_cast<int>(it->getValue()) };
				checkNode(types,++it);
				for (;nb > 0; --nb)
					t = ExprType(type_names.at(it->getValue()),t);
			}
			else
				t = ExprType(type_names.at(it->getValue()),t);
		}

		*this = t;
	}

	ExprType::ExprType(ExprType::Type container, const ExprType& contained_) :
		type{container}, contained{std::make_unique<ExprType>(contained_)}
	{
		assert((type == SYMBOL || type == LIST) && "Not a real container");
	}

	ExprType::ExprType(ExprType::Type container, std::unique_ptr<ExprType> ncontained) :
		type{container}, contained{std::move(ncontained)}
	{
		/* WARNING: this constructors takes the
		 * ownership of contained
		 */
		assert((type == SYMBOL || type == LIST) && "Not a real container");
	}

	ExprType& ExprType::operator = (ExprType::Type t) {
		type = t;
		return *this;
	}

	ExprType& ExprType::operator = (const ExprType& other) {
		type = other.type;
		if (other.isContainer())
			contained = std::make_unique<ExprType>(*other.contained);
		return *this;
	}

	ExprType& ExprType::setContained(ExprType::Type t) {
		/* Set the contained value, even if it is
		 * already set, so be careful.
		 * return a reference to the contained value
		 */
		contained = std::make_unique<ExprType>(t);
		return *contained;
	}


	bool ExprType::operator == (const ExprType& other) const{
		/* true if other is exactly the same
		 * type as *this
		 */
		if (contained) {
			if (other.contained) {
				return type == other.type && *contained == *other.contained;
			} else {
				return false;
			}
		}
		return type == other.type;
	}

	bool ExprType::operator == (const ExprType::Type t) const{
		/* true if t match with the current type
		 */
		if (!contained)
			return type == t;
		return false;
	}


	bool ExprType::operator != (const ExprType& other) const {
		return !(*this == other);
	}

	bool ExprType::operator != (const ExprType::Type t) const{
		return !(*this == t);
	}

	bool ExprType::operator & (const ExprType::Type t) const{
		/* true if t match with type, no matter
		 * what contained is
		 */
		return t == type;
	}

	bool ExprType::isContainer() const {
		/* true if type is a container
		 */
		return static_cast<bool>(contained);
	}

	bool ExprType::isConst() const {
		/* true if type is const
		 */
		return is_const;
	}

	ExprType::Type ExprType::getPrimitive() const {
		/* return primitive type 
		 */
		if (!isContainer())
			return type;
		return contained->getPrimitive();
	}

	ExprType::Type ExprType::getPrimitive(ExprType::Type t) const {
		/* Return a fake primitive type,
		 * i.e. the first type contained which
		 * is not t
		 * t should be a container
		 */
		if (t == type) {
			assert(contained &&"Contained not set");
			return contained->getPrimitive(t);
		}
		return type;
	}

	ExprType::Type ExprType::getType() const {
		return type;
	}

	int ExprType::getDepth() const {
		/* Get the number of containers
		 * necessary to find 
		 * a primitive value
		 */
		if (!isContainer())
			return 0;
		return contained->getDepth() + 1;
	}

	int ExprType::getDepth(ExprType::Type t) const {
		/* Return the number of containers
		 * necessary to find a primitive
		 * value or a value which is not t
		 * t should be a container
		 */
		if (type == t) {
			assert(contained&&"contained not set");
			return contained->getDepth(t) + 1;
		}
		return 0;
	}

	const ExprType& ExprType::getContained() const {
		/* Return the type contained if 
		 * it has one
		 */
		assert(isContainer() && "Type is not a container");
		return *contained;
	}

	ExprType& ExprType::getContained() {
		/* Non const version of the getter
		 */
		return *contained;
	}

	ExprType::operator ExprType::Type () const {
		/* Unsafe operation
		 * which returns the type
		 */
		return type;
	}

	bst::str ExprType::toString(bool asContained) const {
		bst::str s;
		if (!asContained)
			s = "<ExprType>\n";

		s += typeToString(type);
		if (contained)
			s += contained->toString(true);
		if (!asContained)
			s += '\n';

		return s;
	}
	
	void ExprType::checkNode(const NodeVector& types, const NodeVector::const_reverse_iterator it) {
		/* Check that *it is a correct type
		 */
		const bst::str& type_name {it->getValue()};

		if (it->type() == Node::VARIABLE_TYPE) {
			if (type_names.count(type_name) != 1)
				throw exc(exc::UnknownType,"Type not recognized",it->getPosition());

			if (type_names.at(type_name) < ET::MAX_PRIMITIVE && it != types.crbegin())
				throw exc(exc::TypeError,"Primitive type must be at the end of the list of types",it->getPosition());
			if (type_names.at(type_name) < ET::MAX_CONTAINER &&
			   type_names.at(type_name) > ET::MIN_CONTAINER && 
			   it == types.crbegin())
				throw exc(exc::TypeError,"Container should always be declared with a primitive type",it->getPosition());
			// special verifications for void
			if (
			   (type_name == "void" && types.size() == 1) ||
			   (type_name == "list" && (it + 1)->getValue() == "void") ||
			   (type_name == "list" && (it + 1)->type() == Node::LITINT && (it+2)->getValue() == "void")
			   )
				throw exc(exc::TypeError,"Void should be used only with symbols",it->getPosition());

		}

		else if (it->type() == Node::LITINT) {
			if (it == types.crbegin() || it == types.crend() -1)
				throw exc(exc::TypeError,"Type number can't be at the start or at the end of the type declaration",it->getPosition());
			if (type_name == "0")
				throw exc(exc::TypeError,"Type number can't be zero",it->getPosition());
		}
		else
			assert(false&&"Type not recognized: impossible to construct an ExprType");
	}



}
