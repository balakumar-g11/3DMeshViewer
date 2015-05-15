#ifndef C_VECTOR_3D_
#define C_VECTOR_3D_

namespace MeshViewer
{
	namespace Utilities
	{
		namespace Math
		{
			class CVector3D
			{
			public:
				CVector3D(double, double, double);

				CVector3D();

				~CVector3D();

				CVector3D(const CVector3D&);

				CVector3D operator+(CVector3D&);

				CVector3D operator-(CVector3D&);

				void operator=(const CVector3D&);
				
				CVector3D operator*(double&);

				CVector3D operator/(double&);
				
				static double dotProduct(CVector3D&, CVector3D&);

				static CVector3D crossProduct(CVector3D&, CVector3D&);

				double length();

				bool setVertex(double, double, double);

				bool getVertex(double&, double&, double&);

				double x;
				
				double y;

				double z;
			};
		}
	}
}

#endif
