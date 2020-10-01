#pragma once

#define __DEBUG if (GL_NO_ERROR != error) \
	{\
		printf("GL Error %x encountered in.\n", error);\
	};


#define GETTER_REF(type,variable,name) \
	type& Get##name()  \
	{ \
		return variable; \
	};\
	const type& Get##name() const\
	{ \
		return variable; \
	};

#define GETTER(type,variable,name) \
	type Get##name()\
	{ \
		return variable; \
	};\
	type Get##name() const\
	{ \
		return variable; \
	};

#define GET_SET_REF(type,variable,name) \
	type& Get##name()  \
	{ \
		return variable; \
	};\
	void Set##name(type& value) \
	{\
		variable = value;\
	};

#define GET_SET_CONST_REF(type,variable,name) \
	const type& Get##name()  const\
	{ \
		return variable; \
	};\
	void Set##name(type& value) \
	{\
		variable = value;\
	};

#define GET_SET_VAULE(type,variable,name) \
	type Get##name()\
	{ \
		return variable; \
	};\
	type Get##name() const\
	{ \
		return variable; \
	};\
	void Set##name(const type& value) \
	{\
		variable = value;\
	};
