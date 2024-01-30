/*****************************************************************************\
*                                                                           *
* File(s): VideoPlayer.h and VideoPlayer.cpp                            *
*                                                                           *
* Content:    *
*                                                   *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_VIDEOPLAYER_H__
#define __CFORGE_VIDEOPLAYER_H__


#include <crossforge/Core/CForgeObject.h>
#include <crossforge/Graphics/Actors/ScreenQuad.h>
#include <crossforge/Graphics/GLTexture2D.h>

namespace CForge {
	class VideoPlayer : public CForgeObject {
	public:
		VideoPlayer(void);
		~VideoPlayer(void);

		void init(Eigen::Vector2f Position, Eigen::Vector2f Size, Eigen::Vector2i CanvasSize = Eigen::Vector2i::Zero());
		void clear(void);
		void release(void);

		void update(void);
		void render(class RenderDevice* pRDev);

		void canvasSize(const uint32_t Width, const uint32_t Height);

		void play(const std::string VideoFile);
		void play(void); // play same video again

		Eigen::Vector2f position(void)const;
		Eigen::Vector2f size(void)const;

		bool finished(void)const;

	protected:
		struct FrameItem {
			T2DImage<uint8_t> Img; ///< Frame image
			uint32_t FrameIndex;
			uint64_t Timestamp; ///< Timestamp in milliseconds

			FrameItem(void) {
				Img.clear();
				FrameIndex = 0;
				Timestamp = 0;
			}
		};

		void computeDisplayParams(void);
		void readNextFrame(void);
		void getNextFrame(void);
		void cacheVideo(void);

		Eigen::Vector2i m_CanvasSize;
		Eigen::Vector2f m_Position; 
		Eigen::Vector2f m_Size;

		Eigen::Vector3f m_DisplayPosition;
		Eigen::Vector3f m_DisplayScale; 

		std::vector<FrameItem*> m_VideoBuffer;
		uint32_t m_CurrentBufferItem;
		/*uint32_t m_BufferSize;*/

		ScreenQuad m_DisplayQuad;
		GLShader *m_pDisplayQuadShader;

		void* m_pVideoCapture;

		int64_t m_CurrentFrame;
		uint64_t m_VideoStart;
		uint64_t m_FrameTimestamp;
		uint64_t m_NextTimestamp;
		GLTexture2D m_Tex;

		float m_FPS;

		bool m_FinishedPlaying;
	};//VideoPlayer
}

#endif