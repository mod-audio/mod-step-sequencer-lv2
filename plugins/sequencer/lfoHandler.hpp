#ifndef _LFO_HANDLER_
#define _LFO_HANDLER_

class LfoHandler {
public:
	LfoHandler();
	~LfoHandler();
	void applyLfoToParameters();
private:
    float minParamValue[18] = {0, 0, 0, 1, 25, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    float maxParamValue[18] = {0, 10, 0.99, 4, 75, 1, 2, 70, 1, 0, 127, 127, 127, 127, 127, 127, 127, 127};
};

#endif //_LFO_HANDLER_
