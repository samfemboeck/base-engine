#include "QuickMaths.h"
#include <unordered_map>

class SubTexture2D;

struct Animation
{
	Animation(std::string name, unsigned fps, unsigned indexFrom, unsigned indexTo, bool isPingPong = false) :
		Name(name),
		Fps(fps),
		IndexFrom(indexFrom),
		IndexTo(indexTo),
		IndexCur(indexFrom),
		IsPingPong(isPingPong)
	{
	}

	std::string Name;
	float Fps;
	int IndexFrom;
	int IndexTo;
	int IndexCur;
	bool IsPingPong = false;
	bool IsBackLoop = false;
};

class Sprite
{
public:
	void addAnimation(Animation* anim);
	void tick();
	void playAnimation(std::string name, bool isOneShot = false);
	void playAnimationAndFreeze(std::string name);
	void setSheet(Texture2D* texture, glm::ivec2 spriteSize);

public:
	SubTexture2D* SubTexture;

private:
	std::unordered_map<std::string, Animation*> animations_;
	Animation* activeAnimation_ = nullptr;
	Animation* lastAnimation = nullptr;
	bool isOneShot_ = false;
	bool isFreeze_ = false;
	float elapsed_ = 0.0f;
	glm::ivec2 size_;
};