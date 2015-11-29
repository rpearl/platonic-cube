#ifndef _PATTERN_H_
#define _PATTERN_H_

class Pattern {
	public:
		virtual void setup() {};
		virtual void show() = 0;
		virtual void teardown() {};
};

#endif
