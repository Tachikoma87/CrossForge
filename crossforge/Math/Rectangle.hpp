/*****************************************************************************\
*                                                                           *
* File(s): Rectangle.hpp                               *
*                                                                           *
* Content:                    *
*                *
*                               *
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
#ifndef __CFORGE_RECTANGLE_HPP__
#define __CFORGE_RECTANGLE_HPP__

#include "../Core/CForgeObject.h"

namespace CForge {
	class Rectangle : public CForgeObject {
	public:
		Rectangle(void): CForgeObject("Rectangle") {

		}//Constructor

		~Rectangle(void) {

		}//Destructor

		void init(float PosX, float PosY, float Width, float Height) {
			position(Eigen::Vector2f(PosX, PosY));
			size(Eigen::Vector2f(Width, Height));
		}//initialize

		void init(const Eigen::Vector2f Position, const Eigen::Vector2f Size) {
			position(Position);
			size(Size);
		}//initialize

		void position(const Eigen::Vector2f Position) {
			m_Position = Position;
		}//position

		void size(const Eigen::Vector2f Size) {
			m_Size = Size;
		}//size

		Eigen::Vector2f position(void)const {
			return m_Position;
		}//position

		Eigen::Vector2f size(void)const {
			return m_Size;
		}//size

		float width(void)const {
			return m_Size.x();
		}//width

		float height(void)const {
			return m_Size.y();
		}//height

		bool isPointInside(const Eigen::Vector2f Point) {
			return (
				(Point.x() >= m_Position.x()) &&
				(Point.x() >= m_Position.y()) &&
				(Point.x() <= m_Position.x() + m_Size.x()) &&
				(Point.y() <= m_Position.y() + m_Size.y()));

		}//isPointInside

	protected:
		Eigen::Vector2f m_Position;
		Eigen::Vector2f m_Size;
	};//Rectangle

}//name space

#endif