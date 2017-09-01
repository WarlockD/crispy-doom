#include "polygon.h"

void XformAndProjectPObject(PObject * ObjectToXform)
{
	int i, NumPoints = ObjectToXform->NumVerts;
	Point3 * Points = ObjectToXform->VertexList;
	Point3 * XformedPoints = ObjectToXform->XformedVertexList;
	Point3 * ProjectedPoints = ObjectToXform->ProjectedVertexList;
	Point * ScreenPoints = ObjectToXform->ScreenVertexList;

	/* Recalculate the object->view transform */
	ConcatXforms(WorldViewXform, ObjectToXform->XformToWorld,
		ObjectToXform->XformToView);
	/* Apply that new transformation and project the points */
	for (i = 0; i<NumPoints; i++, Points++, XformedPoints++,
		ProjectedPoints++, ScreenPoints++) {
		/* Transform to view space */
		XformVec(ObjectToXform->XformToView, (Fixedpoint *)Points,
			(Fixedpoint *)XformedPoints);
		/* Perspective-project to screen space */
		ProjectedPoints->X =
			FixedMul(FixedDiv(XformedPoints->X, XformedPoints->Z),
				DOUBLE_TO_FIXED(PROJECTION_RATIO * (SCREEN_WIDTH / 2)));
		ProjectedPoints->Y =
			FixedMul(FixedDiv(XformedPoints->Y, XformedPoints->Z),
				DOUBLE_TO_FIXED(PROJECTION_RATIO * (SCREEN_WIDTH / 2)));
		ProjectedPoints->Z = XformedPoints->Z;
		/* Convert to screen coordinates. The Y coord is negated to flip from
		increasing Y being up to increasing Y being down, as expected by polygon
		filler. Add in half the screen width and height to center on screen. */
		ScreenPoints->X = ((int)((ProjectedPoints->X +
			DOUBLE_TO_FIXED(0.5)) >> 16)) + SCREEN_WIDTH / 2;
		ScreenPoints->Y = (-((int)((ProjectedPoints->Y +
			DOUBLE_TO_FIXED(0.5)) >> 16))) + SCREEN_HEIGHT / 2;
	}
}

/* Concatenate a rotation by Angle around the X axis to transformation in
XformToChange, placing the result back into XformToChange. */
void AppendRotationX(Xform XformToChange, double Angle)
{
	Fixedpoint Temp10, Temp11, Temp12, Temp20, Temp21, Temp22;
	Fixedpoint CosTemp = DOUBLE_TO_FIXED(cos(Angle));
	Fixedpoint SinTemp = DOUBLE_TO_FIXED(sin(Angle));

	/* Calculate the new values of the six affected matrix entries */
	Temp10 = FixedMul(CosTemp, XformToChange[1][0]) +
		FixedMul(-SinTemp, XformToChange[2][0]);
	Temp11 = FixedMul(CosTemp, XformToChange[1][1]) +
		FixedMul(-SinTemp, XformToChange[2][1]);
	Temp12 = FixedMul(CosTemp, XformToChange[1][2]) +
		FixedMul(-SinTemp, XformToChange[2][2]);
	Temp20 = FixedMul(SinTemp, XformToChange[1][0]) +
		FixedMul(CosTemp, XformToChange[2][0]);
	Temp21 = FixedMul(SinTemp, XformToChange[1][1]) +
		FixedMul(CosTemp, XformToChange[2][1]);
	Temp22 = FixedMul(SinTemp, XformToChange[1][2]) +
		FixedMul(CosTemp, XformToChange[2][2]);
	/* Put the results back into XformToChange */
	XformToChange[1][0] = Temp10; XformToChange[1][1] = Temp11;
	XformToChange[1][2] = Temp12; XformToChange[2][0] = Temp20;
	XformToChange[2][1] = Temp21; XformToChange[2][2] = Temp22;
}
/* Concatenate a rotation by Angle around the Y axis to transformation in
XformToChange, placing the result back into XformToChange. */
void AppendRotationY(Xform XformToChange, double Angle)
{
	Fixedpoint Temp00, Temp01, Temp02, Temp20, Temp21, Temp22;
	Fixedpoint CosTemp = DOUBLE_TO_FIXED(cos(Angle));
	Fixedpoint SinTemp = DOUBLE_TO_FIXED(sin(Angle));

	/* Calculate the new values of the six affected matrix entries */
	Temp00 = FixedMul(CosTemp, XformToChange[0][0]) +
		FixedMul(SinTemp, XformToChange[2][0]);
	Temp01 = FixedMul(CosTemp, XformToChange[0][1]) +
		FixedMul(SinTemp, XformToChange[2][1]);
	Temp02 = FixedMul(CosTemp, XformToChange[0][2]) +
		FixedMul(SinTemp, XformToChange[2][2]);
	Temp20 = FixedMul(-SinTemp, XformToChange[0][0]) +
		FixedMul(CosTemp, XformToChange[2][0]);
	Temp21 = FixedMul(-SinTemp, XformToChange[0][1]) +
		FixedMul(CosTemp, XformToChange[2][1]);
	Temp22 = FixedMul(-SinTemp, XformToChange[0][2]) +
		FixedMul(CosTemp, XformToChange[2][2]);
	/* Put the results back into XformToChange */
	XformToChange[0][0] = Temp00; XformToChange[0][1] = Temp01;
	XformToChange[0][2] = Temp02; XformToChange[2][0] = Temp20;
	XformToChange[2][1] = Temp21; XformToChange[2][2] = Temp22;
}

/* Concatenate a rotation by Angle around the Z axis to transformation in
XformToChange, placing the result back into XformToChange. */
void AppendRotationZ(Xform XformToChange, double Angle)
{
	Fixedpoint Temp00, Temp01, Temp02, Temp10, Temp11, Temp12;
	Fixedpoint CosTemp = DOUBLE_TO_FIXED(cos(Angle));
	Fixedpoint SinTemp = DOUBLE_TO_FIXED(sin(Angle));

	/* Calculate the new values of the six affected matrix entries */
	Temp00 = FixedMul(CosTemp, XformToChange[0][0]) +
		FixedMul(-SinTemp, XformToChange[1][0]);
	Temp01 = FixedMul(CosTemp, XformToChange[0][1]) +
		FixedMul(-SinTemp, XformToChange[1][1]);
	Temp02 = FixedMul(CosTemp, XformToChange[0][2]) +
		FixedMul(-SinTemp, XformToChange[1][2]);
	Temp10 = FixedMul(SinTemp, XformToChange[0][0]) +
		FixedMul(CosTemp, XformToChange[1][0]);
	Temp11 = FixedMul(SinTemp, XformToChange[0][1]) +
		FixedMul(CosTemp, XformToChange[1][1]);
	Temp12 = FixedMul(SinTemp, XformToChange[0][2]) +
		FixedMul(CosTemp, XformToChange[1][2]);
	/* Put the results back into XformToChange */
	XformToChange[0][0] = Temp00; XformToChange[0][1] = Temp01;
	XformToChange[0][2] = Temp02; XformToChange[1][0] = Temp10;
	XformToChange[1][1] = Temp11; XformToChange[1][2] = Temp12;
}

/* Matrix multiplies Xform by SourceVec, and stores the result in DestVec.
Multiplies a 4x4 matrix times a 4x1 matrix; the result is a 4x1 matrix. Cheats
by assuming the W coord is 1 and bottom row of matrix is 0 0 0 1, and doesn't
bother to set the W coordinate of the destination. */
void XformVec(Xform WorkingXform, Fixedpoint *SourceVec,
	Fixedpoint *DestVec)
{
	int i;

	for (i = 0; i<3; i++)
		DestVec[i] = FixedMul(WorkingXform[i][0], SourceVec[0]) +
		FixedMul(WorkingXform[i][1], SourceVec[1]) +
		FixedMul(WorkingXform[i][2], SourceVec[2]) +
		WorkingXform[i][3];   /* no need to multiply by W = 1 */
}

/* Matrix multiplies SourceXform1 by SourceXform2 and stores result in
DestXform. Multiplies a 4x4 matrix times a 4x4 matrix; result is a 4x4 matrix.
Cheats by assuming bottom row of each matrix is 0 0 0 1, and doesn't bother
to set the bottom row of the destination. */
void ConcatXforms(Xform SourceXform1, Xform SourceXform2,
	Xform DestXform)
{
	int i, j;

	for (i = 0; i<3; i++) {
		for (j = 0; j<4; j++)
			DestXform[i][j] =
			FixedMul(SourceXform1[i][0], SourceXform2[0][j]) +
			FixedMul(SourceXform1[i][1], SourceXform2[1][j]) +
			FixedMul(SourceXform1[i][2], SourceXform2[2][j]) +
			SourceXform1[i][3];
	}
}

/* All vertices in the basic cube */
static IntPoint3 IntCubeVerts[NUM_CUBE_VERTS] = {
	{ 15,15,15 },{ 15,15,-15 },{ 15,-15,15 },{ 15,-15,-15 },
	{ -15,15,15 },{ -15,15,-15 },{ -15,-15,15 },{ -15,-15,-15 } };
/* Transformation from world space into view space (no transformation,
currently) */
static int IntWorldViewXform[3][4] = {
	{ 1,0,0,0 },{ 0,1,0,0 },{ 0,0,1,0 } };

void InitializeFixedPoint()
{
	int i, j;

	for (i = 0; i<3; i++)
		for (j = 0; j<4; j++)
			WorldViewXform[i][j] = INT_TO_FIXED(IntWorldViewXform[i][j]);
	for (i = 0; i<NUM_CUBE_VERTS; i++) {
		CubeVerts[i].X = INT_TO_FIXED(IntCubeVerts[i].X);
		CubeVerts[i].Y = INT_TO_FIXED(IntCubeVerts[i].Y);
		CubeVerts[i].Z = INT_TO_FIXED(IntCubeVerts[i].Z);
	}
}

void RotateAndMovePObject(PObject * ObjectToMove)
{
	if (--ObjectToMove->RDelayCount == 0) {   /* rotate */
		ObjectToMove->RDelayCount = ObjectToMove->RDelayCountBase;
		if (ObjectToMove->Rotate.RotateX != 0.0)
			AppendRotationX(ObjectToMove->XformToWorld,
				ObjectToMove->Rotate.RotateX);
		if (ObjectToMove->Rotate.RotateY != 0.0)
			AppendRotationY(ObjectToMove->XformToWorld,
				ObjectToMove->Rotate.RotateY);
		if (ObjectToMove->Rotate.RotateZ != 0.0)
			AppendRotationZ(ObjectToMove->XformToWorld,
				ObjectToMove->Rotate.RotateZ);
		ObjectToMove->RecalcXform = 1;
	}
	/* Move in Z, checking for bouncing and stopping */
	if (--ObjectToMove->MDelayCount == 0) {
		ObjectToMove->MDelayCount = ObjectToMove->MDelayCountBase;
		ObjectToMove->XformToWorld[2][3] += ObjectToMove->Move.MoveZ;
		if (ObjectToMove->XformToWorld[2][3]>ObjectToMove->Move.MaxZ)
			ObjectToMove->Move.MoveZ = 0; /* stop if close enough */
		ObjectToMove->RecalcXform = 1;
	}
}


void DrawPObject(PObject * ObjectToXform)
{
	int i, j, NumFaces = ObjectToXform->NumFaces, NumVertices;
	int * VertNumsPtr;
	Face * FacePtr = ObjectToXform->FaceList;
	Point * ScreenPoints = ObjectToXform->ScreenVertexList;
	long v1, v2, w1, w2;
	Point Vertices[MAX_POLY_LENGTH];
	PointListHeader Polygon;

	/* Draw each visible face (polygon) of the object in turn */
	for (i = 0; i<NumFaces; i++, FacePtr++) {
		NumVertices = FacePtr->NumVerts;
		/* Copy over the face's vertices from the vertex list */
		for (j = 0, VertNumsPtr = FacePtr->VertNums; j<NumVertices; j++)
			Vertices[j] = ScreenPoints[*VertNumsPtr++];
		/* Draw only if outside face showing (if the normal to the
		polygon points toward viewer; that is, has a positive Z component) */
		v1 = Vertices[1].X - Vertices[0].X;
		w1 = Vertices[NumVertices - 1].X - Vertices[0].X;
		v2 = Vertices[1].Y - Vertices[0].Y;
		w2 = Vertices[NumVertices - 1].Y - Vertices[0].Y;
		if ((v1*w2 - v2*w1) > 0) {
			/* It is facing the screen, so draw */
			/* Appropriately adjust the extent of the rectangle used to
			erase this object later */
			for (j = 0; j<NumVertices; j++) {
				if (Vertices[j].X >
					ObjectToXform->EraseRect.Right)
					if (Vertices[j].X < SCREEN_WIDTH)
						ObjectToXform->EraseRect.Right =
						Vertices[j].X;
					else ObjectToXform->EraseRect.Right =
						SCREEN_WIDTH;
				if (Vertices[j].Y >
					ObjectToXform->EraseRect.Bottom)
					if (Vertices[j].Y < SCREEN_HEIGHT)
						ObjectToXform->EraseRect.Bottom =
						Vertices[j].Y;
					else ObjectToXform->EraseRect.Bottom =
						SCREEN_HEIGHT;
				if (Vertices[j].X <
					ObjectToXform->EraseRect.Left)
					if (Vertices[j].X > 0)
						ObjectToXform->EraseRect.Left =
						Vertices[j].X;
					else ObjectToXform->EraseRect.Left = 0;
					if (Vertices[j].Y <
						ObjectToXform->EraseRect.Top)
						if (Vertices[j].Y > 0)
							ObjectToXform->EraseRect.Top =
							Vertices[j].Y;
						else ObjectToXform->EraseRect.Top = 0;
			}
			/* Draw the polygon */
			DRAW_POLYGON(Vertices, NumVertices, FacePtr->Color, 0, 0);
		}
	}
}
