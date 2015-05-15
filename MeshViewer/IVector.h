#include "CDataTypes.h"

namespace MeshViewer
{
	namespace Utilities
	{
		namespace Math
		{
			class IVector
			{
			public:
				virtual IVector& operator+(IN IVector&) = 0;

				virtual IVector& operator-(IN IVector&) = 0;

				virtual IVector& operator*(IN IVector&) = 0;

				virtual bool dotProduct(IN IVector&, float&) = 0;

				virtual bool crossProduct(IN IVector&, OUT IVector&) = 0;
			};
		}
	}
}