#include "../pch.h"
#include "Sprite.h"
#include "Time.h"
#include "Renderer2D.h"
#include "OrthoCamController.h"

void Sprite::addAnimation(Animation* anim)
{
	animations_[anim->Name] = anim;
}

void Sprite::playAnimation(std::string name, bool isOneShot)
{
	isFreeze_ = false;
	elapsed_ = 0;

	if (activeAnimation_)
		activeAnimation_->IndexCur = activeAnimation_->IndexFrom;

	lastAnimation = activeAnimation_;
	activeAnimation_ = animations_[name];
	isOneShot_ = isOneShot;
}

void Sprite::playAnimationAndFreeze(std::string name)
{
	elapsed_ = 0;
	activeAnimation_->IndexCur = activeAnimation_->IndexFrom;
	lastAnimation = activeAnimation_;
	activeAnimation_ = animations_[name];
	isFreeze_ = true;
}

void Sprite::setSheet(Texture2D* texture, glm::ivec2 spriteSize)
{
	SubTexture = new SubTexture2D(*texture, spriteSize);
	size_ = spriteSize;
}

void Sprite::tick()
{
	elapsed_ += Time::DeltaSeconds;

	if (elapsed_ > 1.0f / activeAnimation_->Fps)
	{
		elapsed_ = 0;

		if (activeAnimation_->IsPingPong)
		{
			if (activeAnimation_->IsBackLoop)
			{
				activeAnimation_->IndexCur--;

				if (activeAnimation_->IndexCur < activeAnimation_->IndexFrom)
				{
					activeAnimation_->IsBackLoop = false;
					activeAnimation_->IndexCur = activeAnimation_->IndexFrom + 1;
				}
			}
			else
			{
				activeAnimation_->IndexCur++;

				if (activeAnimation_->IndexCur > activeAnimation_->IndexTo)
				{
					activeAnimation_->IsBackLoop = true;
					activeAnimation_->IndexCur = activeAnimation_->IndexTo - 1;
				}
			}
		}
		else if (isOneShot_)
		{
			isOneShot_ = false;
			activeAnimation_ = lastAnimation;
		}
		else
		{
			activeAnimation_->IndexCur++;

			if (activeAnimation_->IndexCur > activeAnimation_->IndexTo)
			{
				if (isFreeze_)
					return;

				activeAnimation_->IndexCur = activeAnimation_->IndexFrom;
			}
		}

		unsigned textureWidth = SubTexture->getTexture().getWidth();
		int xCoord = activeAnimation_->IndexCur * size_.x % textureWidth;
		int yCoord = activeAnimation_->IndexCur * size_.x / textureWidth * size_.y;
		SubTexture->setCoords({ xCoord, yCoord });
	}
}
