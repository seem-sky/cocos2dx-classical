/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-better
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCShake.h"
#include "cocoa/CCZone.h"
#include "support/CCPointExtension.h"

NS_CC_BEGIN

CCShake::~CCShake() {
}

CCShake* CCShake::create(float duration, float maxRadius, float minRadius, float shakeInterval) {
	CCShake* a = new CCShake();
	a->initWithRing(duration, maxRadius, minRadius, shakeInterval);
	CC_SAFE_AUTORELEASE_RETURN(a, CCShake*);
}

CCShake* CCShake::create(float duration, float startMaxR, float endMaxR, float startMinR, float endMinR, float shakeInterval) {
    CCShake* a = new CCShake();
	a->initWithDynamicRing(duration, startMaxR, endMaxR, startMinR, endMinR, shakeInterval);
	CC_SAFE_AUTORELEASE_RETURN(a, CCShake*);
}

bool CCShake::initWithRing(float d, float maxR, float minR, float shakeInterval) {
    m_mode = RING;
	m_fDuration = d;
    m_maxRadius = maxR;
    m_minRadius = minR;
    m_endMaxRadius = maxR;
    m_endMinRadius = minR;
    m_shakeInterval = shakeInterval;
	return true;
}

bool CCShake::initWithDynamicRing(float d, float startMaxR, float endMaxR, float startMinR, float endMinR, float shakeInterval) {
    m_mode = DYNAMIC_RING;
	m_fDuration = d;
    m_maxRadius = startMaxR;
    m_minRadius = startMinR;
    m_endMaxRadius = endMaxR;
    m_endMinRadius = endMinR;
    m_shakeInterval = shakeInterval;
	return true;
}

CCObject* CCShake::copyWithZone(CCZone *pZone) {
    CCZone* pNewZone = nullptr;
    CCShake* pCopy = nullptr;
    if(pZone && pZone->m_pCopyObject) {
        // in case of being called at sub class
        pCopy = (CCShake*)(pZone->m_pCopyObject);
    } else {
        pCopy = new CCShake();
        pZone = pNewZone = new CCZone(pCopy);
        CC_SAFE_AUTORELEASE(pCopy);
    }
	
    CCActionInterval::copyWithZone(pZone);
    switch (m_mode) {
        case DYNAMIC_RING:
            pCopy->initWithDynamicRing(m_fDuration, m_maxRadius, m_endMaxRadius, m_minRadius, m_endMinRadius, m_shakeInterval);
            break;
        default:
            pCopy->initWithRing(m_fDuration, m_maxRadius, m_minRadius, m_shakeInterval);
            break;
    }
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

CCActionInterval* CCShake::reverse() {
    switch (m_mode) {
        case DYNAMIC_RING:
            return create(m_fDuration, m_maxRadius, m_endMaxRadius, m_minRadius, m_endMinRadius, m_shakeInterval);
        default:
            return create(m_fDuration, m_maxRadius, m_minRadius, m_shakeInterval);
    }
}

void CCShake::update(float t) {
	if(t >= 1) {
		getTarget()->setPosition(m_originalX, m_originalY);
	} else {
        m_time += t;
        switch (m_mode) {
            case RING:
            case DYNAMIC_RING:
            {
                if(m_shakeInterval == 0 || m_time > m_shakeInterval) {
                    float radian = CCRANDOM_0_1() * M_PI * 2;
                    float maxR = m_maxRadius + (m_endMinRadius - m_maxRadius) * t;
                    float minR = m_minRadius + (m_endMinRadius - m_minRadius) * t;
                    float r = CCRANDOM_0_1() * (maxR - minR) + minR;
                    float x = m_originalX + cosf(radian) * r;
                    float y = m_originalY + sinf(radian) * r;
                    getTarget()->setPosition(ccp(x, y));
                    
                    if(m_shakeInterval > 0) {
                        m_time = fmodf(m_time, m_shakeInterval);
                    }
                }
                break;
            }
            default:
                break;
        }
	}
}

void CCShake::startWithTarget(CCNode* pTarget) {
	CCActionInterval::startWithTarget(pTarget);

	m_originalX = pTarget->getPositionX();
	m_originalY = pTarget->getPositionY();
    m_time = 0;
}

NS_CC_END