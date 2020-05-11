#ifndef _H_SAMPLE_SELECTOR_
#define _H_SAMPLE_SELECTOR_

#include "Application.h"
#include <vector>

class SampleSelector {
protected:
	Application* mCurrentSample;
	bool mIsRunning;
private:
	SampleSelector(const SampleSelector&);
	SampleSelector& operator=(const SampleSelector&);
public:
	SampleSelector();
	~SampleSelector();

	void ImGui(nk_context* context, float xPos);
	Application* GetCurrentSample();

	void Initialize();
	void Shutdown();
	bool IsRunning();
};

#endif