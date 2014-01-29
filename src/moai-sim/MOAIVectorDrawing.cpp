// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIGfxDevice.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIVectorCombo.h>
#include <moai-sim/MOAIVectorEllipse.h>
#include <moai-sim/MOAIVectorDrawing.h>
#include <moai-sim/MOAIVectorPoly.h>
#include <moai-sim/MOAIVectorPoly.h>
#include <moai-sim/MOAIVectorRect.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <tesselator.h>

//================================================================//
// MOAIVectorDrawingVertexWriter
//================================================================//
class MOAIVectorDrawingVertexWriter :
	public ZLAbstractVertexWriter2D {
private:

	MOAIVectorDrawing* mDrawing;

public:

	SET ( MOAIVectorDrawing*, Drawing, mDrawing )

	//----------------------------------------------------------------//
	void WriteVertex ( const ZLVec2D& v ) {
		assert ( this->mDrawing );
		this->mDrawing->PushVertex ( v.mX, v.mY );
	}
};

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
int MOAIVectorDrawing::_clearTransforms ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->ClearTransforms ();
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_drawingToWorld ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetValue ( 2, 0.0f );
	vec.mY = state.GetValue ( 3, 0.0f );

	self->mStyle.mDrawingToWorld.Transform ( vec );
	
	state.Push ( vec.mX );
	state.Push ( vec.mY );
	
	return 2;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_drawingToWorldVec ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetValue ( 2, 0.0f );
	vec.mY = state.GetValue ( 3, 0.0f );

	self->mStyle.mDrawingToWorld.TransformVec ( vec );
	
	state.Push ( vec.mX );
	state.Push ( vec.mY );
	
	return 2;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_finish ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->Finish ();
	
	bool hasContent = self->mVtxBuffer.GetVertexCount () > 0;
	state.Push ( hasContent );
	return 1;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushBezierVertices ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	ZLVec2D p0;
	ZLVec2D p1;
	ZLVec2D p2;
	ZLVec2D p3;
	
	p0.mX			= state.GetValue < float >( 2, 0.0f );
	p0.mY			= state.GetValue < float >( 3, 0.0f );
	
	p1.mX			= state.GetValue < float >( 4, 0.0f );
	p1.mY			= state.GetValue < float >( 5, 0.0f );
	
	p2.mX			= state.GetValue < float >( 6, 0.0f );
	p2.mY			= state.GetValue < float >( 7, 0.0f );
	
	p3.mX			= state.GetValue < float >( 8, 0.0f );
	p3.mY			= state.GetValue < float >( 9, 0.0f );
	
	self->PushBezierVertices ( p0, p1, p2, p3 );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushCombo ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->PushCombo ();
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushEllipse ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float x			= state.GetValue < float >( 2, 0.0f );
	float y			= state.GetValue < float >( 3, 0.0f );
	float xRad		= state.GetValue < float >( 4, 0.0f );
	float yRad		= state.GetValue < float >( 5, xRad );
	
	self->PushEllipse ( x, y, xRad, yRad );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushPoly ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	ZLVec2D* vertices = 0;
	u32 total = ( state.GetTop () - 1 ) >> 1;
	bool closed = state.GetValue < bool >( -1, true );
	
	if ( total ) {
		vertices = ( ZLVec2D* )alloca ( total * sizeof ( ZLVec2D ));
		
		for ( u32 i = 0; i < total; ++i ) {
			vertices [ i ].mX = state.GetValue < float >(( i << 1 ) + 2, 0 );
			vertices [ i ].mY = state.GetValue < float >(( i << 1 ) + 3, 0 );
		}
	}
	
	self->PushPoly ( vertices, total, closed );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushRect ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	ZLRect rect = state.GetRect < float >( 2 );
	self->PushRect ( rect.mXMin, rect.mYMin, rect.mXMax, rect.mYMax );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushRotate ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float x		= state.GetValue < float >( 2, 0.0f );
	float y		= state.GetValue < float >( 3, 0.0f );
	float r		= state.GetValue < float >( 4, 0.0f );

	self->PushRotate ( x, y, r * ( float )D2R );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushScale ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float x			= state.GetValue < float >( 2, 1.0f );
	float y			= state.GetValue < float >( 3, 1.0f );
	
	self->PushScale ( x, y );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushSkew ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float yx		= state.GetValue < float >( 2, 0.0f );
	float xy		= state.GetValue < float >( 3, 0.0f );
	
	self->PushSkew ( yx, xy );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushTransform ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float a			= state.GetValue < float >( 2, 1.0f );
	float b			= state.GetValue < float >( 3, 0.0f );
	float c			= state.GetValue < float >( 4, 0.0f );
	float d			= state.GetValue < float >( 5, 1.0f );
	
	float tx		= state.GetValue < float >( 6, 0.0f );
	float ty		= state.GetValue < float >( 7, 0.0f );
	
	self->PushTransform ( a, b, c, d, tx, ty );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushTranslate ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float x			= state.GetValue < float >( 2, 1.0f );
	float y			= state.GetValue < float >( 3, 0.0f );
	
	self->PushTranslate ( x, y );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_pushVertex ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	float x		= state.GetValue < float >( 2, 0.0f );
	float y		= state.GetValue < float >( 3, 0.0f );
	
	self->PushVertex ( x, y );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_reserveVertexExtras ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	u32 total		= state.GetValue < u32 >( 2, 0 );
	size_t size		= state.GetValue < u32 >( 3, 0 );
	
	self->ReserveVertexExtras ( total, size );
	return 0;	
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setCapStyle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mCapStyle = state.GetValue < u32 >( 2, MOAIVectorStyle::CAP_BUTT );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setCircleResolution ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mCircleResolution = state.GetValue < u32 >( 2, MOAIVectorStyle::DEFAULT_CIRCLE_RESOLUTION );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setDepthBias ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mDepthBias = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setExtrude ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mExtrude = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setFillColor ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mFillColor = state.GetColor ( 2, 1.0f, 1.0f, 1.0f, 1.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setFillStyle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mFillStyle = state.GetValue < u32 >( 2, MOAIVectorStyle::FILL_NONE );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setFillExtra ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mFillExtraID = state.GetValue < u32 >( 2, 1 ) - 1;
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setJoinStyle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mJoinStyle = state.GetValue < u32 >( 2, MOAIVectorStyle::JOIN_MITER );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLightColor ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mLightColor = state.GetColor ( 2, 1.0f, 1.0f, 1.0f, 1.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLightCurve ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mLightCurve = state.GetValue < u32 >( 2, ZLInterpolate::kLinear );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLightVec ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mLightVec.mX = state.GetValue < float >( 2, 0.0f );
	self->mStyle.mLightVec.mY = state.GetValue < float >( 3, 0.0f );
	self->mStyle.mLightVec.mZ = state.GetValue < float >( 4, 0.0f );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLineColor ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mLineColor = state.GetColor ( 2, 1.0f, 1.0f, 1.0f, 1.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLineStyle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mLineStyle = state.GetValue < u32 >( 2, MOAIVectorStyle::LINE_NONE );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setLineWidth ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mLineWidth = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setMiterLimit ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mMiterLimit = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setPolyClosed ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mPolyClosed = state.GetValue < bool >( 2, false );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setShadowColor ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mShadowColor = state.GetColor ( 2, 0.0f, 0.0f, 0.0f, 1.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setShadowCurve ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mShadowCurve = state.GetValue < u32 >( 2, ZLInterpolate::kLinear );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setStrokeColor ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mStyle.mStrokeColor = state.GetColor ( 2, 1.0f, 1.0f, 1.0f, 1.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setStrokeDepthBias ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mStrokeDepthBias = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setStrokeExtra ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mStrokeExtraID =  state.GetValue < u32 >( 2, 1 ) - 1;
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setStrokeStyle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mStrokeStyle = state.GetValue < u32 >( 2, MOAIVectorStyle::STROKE_CENTER );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setStrokeWidth ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mStrokeWidth = state.GetValue < float >( 2, 0.0f );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setVerbose ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )

	self->mVerbose = state.GetValue < bool >( 2, false );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setVertexExtra ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	u32 idx = state.GetValue < u32 >( 2, 1 )- 1;
	
	size_t len;
	void* extra = ( void* )lua_tolstring ( state, 3, &len );
	
	self->SetVertexExtra ( idx, extra, len );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setVertexFormat ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	MOAIVertexFormat* format = state.GetLuaObject < MOAIVertexFormat >( 2, true );
	self->mVtxBuffer.SetFormat ( format );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_setWindingRule ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "U" )
	
	self->mStyle.mWindingRule = state.GetValue < u32 >( 2, ( u32 )TESS_WINDING_ODD );
	return 0;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_worldToDrawing ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetValue ( 2, 0.0f );
	vec.mY = state.GetValue ( 3, 0.0f );

	self->mStyle.mWorldToDrawing.Transform ( vec );
	
	state.Push ( vec.mX );
	state.Push ( vec.mY );
	
	return 2;
}

//----------------------------------------------------------------//
int MOAIVectorDrawing::_worldToDrawingVec ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIVectorDrawing, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetValue ( 2, 0.0f );
	vec.mY = state.GetValue ( 3, 0.0f );

	self->mStyle.mWorldToDrawing.TransformVec ( vec );
	
	state.Push ( vec.mX );
	state.Push ( vec.mY );
	
	return 2;
}

//================================================================//
// MOAIVectorDrawing
//================================================================//

//----------------------------------------------------------------//
void MOAIVectorDrawing::Clear () {

	for ( u32 i = 0; i < this->mDirectory.GetTop (); ++i ) {
		MOAIVectorShape* shape = this->mDirectory [ i ];
		if ( shape ) {
			delete shape;
		}
	}
	
	this->mDirectory.Clear ();
	this->mShapeStack.Clear ();
	
	this->mIdxStream.Clear ();
	this->mVtxStream.Clear ();
	
	for ( u32 i = 0; i < this->mVtxExtras.Size (); ++i ) {
		free ( this->mVtxExtras [ i ]);
	}
	this->mVtxExtraSize = 0;
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::ClearTransforms () {

	this->mMatrixStack.Reset ();
	this->mStyle.mDrawingToWorld.Ident ();
	this->mStyle.mWorldToDrawing.Ident ();
}

//----------------------------------------------------------------//
u32 MOAIVectorDrawing::CountVertices () {

	return ( this->mVtxStream.GetLength () / MOAIVertexFormatMgr::Get ().GetVertexSize ( MOAIVertexFormatMgr::XYZC ));
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::Draw () {

	if ( this->mVtxBuffer.Bind ()) {
		if ( this->mIdxBuffer.LoadGfxState ()) {
			zglDrawElements ( ZGL_PRIM_TRIANGLES, this->mIdxBuffer.GetIndexCount (), ZGL_TYPE_UNSIGNED_INT, 0 );
		}
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::Finish () {

	u32 vertsTop = this->mVertexStack.GetTop ();
	u32 shapesTop = this->mShapeStack.GetTop ();
	
	if ( vertsTop ) {
	
		assert ( shapesTop );
		MOAIVectorShape* shape = this->mShapeStack [ shapesTop - 1 ];
		bool result = shape->SetVertices ( this->mVertexStack, vertsTop, this->mPolyClosed );
		
		assert ( result );
		UNUSED ( result );
		
		this->mVertexStack.Reset ();
	}
	else {

		if ( shapesTop >= 2 ) {
			
			bool done = false;
			for ( int i = ( shapesTop - 2 ); ( i >= 0 ) && ( !done ); --i ) {
				int c = i + 1;
				MOAIVectorShape* shape = this->mShapeStack [ i ];
				if ( shape->CanGroup () && shape->GroupShapes ( &this->mShapeStack [ c ], shapesTop - c )) {
					this->mShapeStack.SetTop ( c );
					done = true;
				}
			}
		}
		
		if ( this->mShapeStack.GetTop () == shapesTop ) {
			this->mVertexStack.Clear ();
			this->Tessalate ();
		}
	}
}

//----------------------------------------------------------------//
MOAIVectorDrawing::MOAIVectorDrawing () :
	mDepthBias ( 0.0f ),
	mDepthOffset ( 0.0f ),
	mVerbose ( false ),
	mVtxExtraSize ( 0 ) {
	
	this->mStyle.Default ();
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILuaObject )
	RTTI_END
	
	// TODO: make this into a template for composing Lua-bound classes and for safe use on the stack
	//this->mVtxBuffer.Retain (); // prefent the vertex buffer from being deleted by gc or ref count
	//this->mVtxBuffer.SetFormat ( &MOAIVertexFormatMgr::Get ().GetPreset ( MOAIVertexFormatMgr::XYZC ));
}

//----------------------------------------------------------------//
MOAIVectorDrawing::~MOAIVectorDrawing () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PopTransform () {

	this->mMatrixStack.Pop ();
	
	ZLAffine2D transform;
	transform.Ident ();
	
	for ( u32 i = 0; i < this->mMatrixStack.GetTop (); ++i ) {
		transform.Prepend ( this->mMatrixStack [ i ]);
	}
	this->mStyle.mDrawingToWorld = transform;
	this->mStyle.mWorldToDrawing.Inverse ( this->mStyle.mDrawingToWorld );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushBezierVertices ( const ZLVec2D& p0, const ZLVec2D& p1, const ZLVec2D& p2, const ZLVec2D& p3 ) {

	MOAIVectorDrawingVertexWriter writer;
	writer.SetDrawing ( this );

	ZLCubicBezier2D curve;
	curve.Init ( p0, p1, p2, p3 );
	curve.Flatten ( writer );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushCombo () {

	MOAIVectorCombo* combo = new MOAIVectorCombo ();
	this->PushShape ( combo );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushEllipse ( float x, float y, float xRad, float yRad ) {

	MOAIVectorEllipse* ellipse = new MOAIVectorEllipse ();
	this->PushShape ( ellipse );
	ellipse->Init ( x, y, xRad, yRad );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushPoly ( ZLVec2D* vertices, u32 total, bool closed ) {

	this->mPolyClosed = closed;

	MOAIVectorPoly* poly = new MOAIVectorPoly ();
	this->PushShape ( poly );
	poly->SetVertices ( vertices, total, closed );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushRect ( float xMin, float yMin, float xMax, float yMax ) {

	MOAIVectorRect* vectorRect = new MOAIVectorRect ();
	this->PushShape ( vectorRect );
	vectorRect->Init ( xMin, yMin, xMax, yMax );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushRotate ( float x, float y, float r ) {

	ZLAffine2D transform;

	if (( x != 0.0f ) || ( y != 0.0f )) {
		
		ZLAffine2D mtx;
		
		transform.Translate ( -x, -y );
		
		mtx.Rotate ( r );
		transform.Append ( mtx );
	
		mtx.Translate ( x, y );
		transform.Append ( mtx );
		
		this->PushTransform ( transform );
	}
	else {
		transform.Rotate ( r );
		this->PushTransform ( transform );
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushScale ( float x, float y ) {
	
	ZLAffine2D transform;
	transform.Scale ( x, y );
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
u32 MOAIVectorDrawing::PushShape ( MOAIVectorShape* shape ) {

	shape->mStyle = this->mStyle;

	u32 tag = this->mDirectory.GetTop ();

	this->mDirectory.Push ( shape );
	this->mShapeStack.Push ( shape );
	
	return tag;
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushSkew ( float yx, float xy ) {

	ZLAffine2D transform;
	transform.Shear ( Tan ( yx ), Tan ( xy ));
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushTransform ( const ZLAffine2D& transform ) {

	this->mMatrixStack.Push ( transform );
	this->mStyle.mDrawingToWorld.Prepend ( transform );
	this->mStyle.mWorldToDrawing.Inverse ( this->mStyle.mDrawingToWorld );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushTransform ( float a, float b, float c, float d, float tx, float ty ) {

	ZLAffine2D transform;
	
	transform.m [ ZLAffine2D::C0_R0 ] = a;
	transform.m [ ZLAffine2D::C0_R1 ] = b;
	
	transform.m [ ZLAffine2D::C1_R0 ] = c;
	transform.m [ ZLAffine2D::C1_R1 ] = d;
	
	transform.m [ ZLAffine2D::C2_R0 ] = tx;
	transform.m [ ZLAffine2D::C2_R1 ] = ty;
	
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushTranslate ( float x, float y ) {

	ZLAffine2D transform;
	transform.Translate ( x, y );
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::PushVertex ( float x, float y ) {

	ZLVec2D vertex ( x, y );
	this->mVertexStack.Push ( vertex );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::RegisterLuaClass ( MOAILuaState& state ) {
	
	state.SetField ( -1, "FILL_NONE",					( u32 )MOAIVectorStyle::FILL_NONE );
	state.SetField ( -1, "FILL_SOLID",					( u32 )MOAIVectorStyle::FILL_SOLID );
	
	state.SetField ( -1, "LINE_NONE",					( u32 )MOAIVectorStyle::LINE_NONE );
	state.SetField ( -1, "LINE_VECTOR",					( u32 )MOAIVectorStyle::LINE_VECTOR );
	
	state.SetField ( -1, "STROKE_NONE",					( u32 )MOAIVectorStyle::STROKE_NONE );
	state.SetField ( -1, "STROKE_CENTER",				( u32 )MOAIVectorStyle::STROKE_CENTER );
	state.SetField ( -1, "STROKE_INTERIOR",				( u32 )MOAIVectorStyle::STROKE_INTERIOR );
	state.SetField ( -1, "STROKE_EXTERIOR",				( u32 )MOAIVectorStyle::STROKE_EXTERIOR );
	
	state.SetField ( -1, "JOIN_BEVEL",					( u32 )MOAIVectorStyle::JOIN_BEVEL );
	state.SetField ( -1, "JOIN_MITER",					( u32 )MOAIVectorStyle::JOIN_MITER );
	state.SetField ( -1, "JOIN_ROUND",					( u32 )MOAIVectorStyle::JOIN_ROUND );
	
	state.SetField ( -1, "CAP_BUTT",					( u32 )MOAIVectorStyle::CAP_BUTT );
	state.SetField ( -1, "CAP_ROUND",					( u32 )MOAIVectorStyle::CAP_ROUND );
	state.SetField ( -1, "CAP_POINTY",					( u32 )MOAIVectorStyle::CAP_POINTY );
	state.SetField ( -1, "CAP_SQUARE",					( u32 )MOAIVectorStyle::CAP_SQUARE );
	
	state.SetField ( -1, "TESS_WINDING_ODD",			( u32 )TESS_WINDING_ODD );
	state.SetField ( -1, "TESS_WINDING_NONZERO",		( u32 )TESS_WINDING_NONZERO );
	state.SetField ( -1, "TESS_WINDING_POSITIVE",		( u32 )TESS_WINDING_POSITIVE );
	state.SetField ( -1, "TESS_WINDING_NEGATIVE",		( u32 )TESS_WINDING_NEGATIVE );
	state.SetField ( -1, "TESS_WINDING_ABS_GEQ_TWO",	( u32 )TESS_WINDING_ABS_GEQ_TWO );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "clearTransforms",		_clearTransforms },
		{ "drawingToWorld",			_drawingToWorld },
		{ "drawingToWorldVec",		_drawingToWorldVec },
		{ "finish",					_finish },
		{ "pushBezierVertices",		_pushBezierVertices },
		{ "pushCombo",				_pushCombo },
		{ "pushEllipse",			_pushEllipse },
		{ "pushPoly",				_pushPoly },
		{ "pushRect",				_pushRect },
		{ "pushRotate",				_pushRotate },
		{ "pushScale",				_pushScale },
		{ "pushSkew",				_pushSkew },
		{ "pushTransform",			_pushTransform },
		{ "pushTranslate",			_pushTranslate },
		{ "pushVertex",				_pushVertex },
		{ "reserveVertexExtras",	_reserveVertexExtras },
		{ "setCapStyle",			_setCapStyle },
		{ "setCircleResolution",	_setCircleResolution },
		{ "setDepthBias",			_setDepthBias },
		{ "setExtrude",				_setExtrude },
		{ "setFillColor",			_setFillColor },
		{ "setFillStyle",			_setFillStyle },
		{ "setFillExtra",			_setFillExtra },
		{ "setJoinStyle",			_setJoinStyle },
		{ "setLightColor",			_setLightColor },
		{ "setLightCurve",			_setLightCurve },
		{ "setLightVec",			_setLightVec },
		{ "setLineColor",			_setLineColor },
		{ "setLineStyle",			_setLineStyle },
		{ "setLineWidth",			_setLineWidth },
		{ "setMiterLimit",			_setMiterLimit },
		{ "setPolyClosed",			_setPolyClosed },
		{ "setShadowColor",			_setShadowColor },
		{ "setShadowCurve",			_setShadowCurve },
		{ "setStrokeColor",			_setStrokeColor },
		{ "setStrokeDepthBias",		_setStrokeDepthBias },
		{ "setStrokeExtra",			_setStrokeExtra },
		{ "setStrokeStyle",			_setStrokeStyle },
		{ "setStrokeWidth",			_setStrokeWidth },
		{ "setVerbose",				_setVerbose },
		{ "setVertexExtra",			_setVertexExtra },
		{ "setVertexFormat",		_setVertexFormat },
		{ "setWindingRule",			_setWindingRule },
		{ "worldToDrawing",			_worldToDrawing },
		{ "worldToDrawingVec",		_worldToDrawingVec },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::ReserveVertexExtras ( u32 total, size_t size ) {

	this->mVtxExtraSize = size;
	this->mVtxExtras.Init ( total );
	
	for ( u32 i = 0; i < this->mVtxExtras.Size (); ++i ) {
		this->mVtxExtras [ i ] = calloc ( 1, this->mVtxExtraSize );
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::SetVertexExtra ( u32 idx, void* extra, size_t size ) {

	size = size <= this->mVtxExtraSize ? size : this->mVtxExtraSize;
	if ( idx < this->mVtxExtras.Size ()) {
		memcpy ( this->mVtxExtras [ idx ], extra, size );
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::Tessalate () {

	mDepthOffset = 0.0f;

	this->mIdxStream.Clear ();
	this->mVtxStream.Clear ();

	this->mIdxBuffer.Clear ();
	this->mVtxBuffer.Clear ();

	for ( u32 i = 0; i < this->mShapeStack.GetTop (); ++i ) {
		MOAIVectorShape* shape = this->mShapeStack [ i ];
		shape->Tessalate ( *this );
	}
	
	this->mIdxStream.Seek ( 0, SEEK_SET );
	this->mVtxStream.Seek ( 0, SEEK_SET );
	
	this->mIdxBuffer.ReserveIndices ( this->mIdxStream.GetLength () >> 2 );
	this->mIdxBuffer.GetStream ().WriteStream ( this->mIdxStream );
	
	this->mVtxBuffer.SetFormat ( &MOAIVertexFormatMgr::Get ().GetPreset ( MOAIVertexFormatMgr::XYZC ));
	this->mVtxBuffer.Reserve ( this->mVtxStream.GetLength ());
	this->mVtxBuffer.GetStream ().WriteStream ( this->mVtxStream );
	this->mVtxBuffer.Bless ();
	
	this->mIdxStream.Clear ();
	this->mVtxStream.Clear ();
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::WriteContourIndices ( TESStesselator* tess, u32 base ) {

	const int* elems = tessGetElements ( tess );
	const int nelems = tessGetElementCount ( tess );
	
	for ( int i = 0; i < nelems; ++i ) {
		int b = elems [( i * 2 )];
		int n = elems [( i * 2 ) + 1 ];
		
		for ( int j = 0; j < n; ++j ) {
			this->mIdxStream.Write < u32 >( base + b + j );
		}
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::WriteSkirt ( TESStesselator* tess, const MOAIVectorStyle& style, const ZLColorVec& fillColor, u32 vertexExtraID ) {

	u32 base = this->CountVertices ();
	float z = style.GetExtrude ();

	ZLVec3D lightVec = style.GetLightVec ();
	float lightLen = lightVec.Norm ();
	bool doLighting = lightLen != 0.0f ? true : false;
	
	ZLColorVec lightColor = style.mLightColor;
	u32 lightCurve = style.mLightCurve;
	float lightAlpha = lightColor.mA;
	lightColor.mA = 1.0f;

	ZLColorVec shadowColor = style.mShadowColor;
	u32 shadowCurve = style.mShadowCurve;
	float shadowAlpha = shadowColor.mA;
	shadowColor.mA = 1.0f;

	u32 color32 = fillColor.PackRGBA ();

	const int* elems = tessGetElements ( tess );
	const int nelems = tessGetElementCount ( tess );
	const float* verts = tessGetVertices ( tess );
	
	for ( int i = 0; i < nelems; ++i ) {
		int b = elems [( i * 2 )];
		int n = elems [( i * 2 ) + 1 ];
		
		for ( int j = 0; j < n; ++j ) {
			
			ZLVec2D& v0 = (( ZLVec2D* )verts )[ b + j ];
			ZLVec2D& v1 = (( ZLVec2D* )verts )[ b + (( j + 1 ) % n )];
			
			if ( doLighting ) {
			
				ZLVec2D e = v1;
				e.Sub ( v0 );
				e.Rotate90Clockwise ();
				e.Norm ();
				
				ZLVec3D n ( e.mX, e.mY, 0.0f );
				
				ZLColorVec color;
				
				float dot = lightVec.Dot ( n );
				if ( dot < 0.0f ) {
					color.Lerp ( shadowCurve, fillColor, shadowColor, -dot * shadowAlpha );
				}
				else {
					color.Lerp ( lightCurve, fillColor, lightColor, dot * lightAlpha );
				}
				
				color32 = color.PackRGBA ();
			}
			
			this->WriteVertex ( v0.mX, v0.mY, 0.0f, color32, vertexExtraID );
			this->WriteVertex ( v1.mX, v1.mY, 0.0f, color32, vertexExtraID );
			this->WriteVertex ( v0.mX, v0.mY, z, color32, vertexExtraID );
			this->WriteVertex ( v1.mX, v1.mY, z, color32, vertexExtraID );
			
			this->mIdxStream.Write < u32 >( base + 0 );
			this->mIdxStream.Write < u32 >( base + 1 );
			this->mIdxStream.Write < u32 >( base + 3 );
			
			this->mIdxStream.Write < u32 >( base + 0 );
			this->mIdxStream.Write < u32 >( base + 3 );
			this->mIdxStream.Write < u32 >( base + 2 );
			
			base += 4;
		}
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::WriteTriangleIndices ( TESStesselator* tess, u32 base ) {

	if ( this->mVerbose ) {
		MOAIPrint ( "WRITING INDICES:\n" );
	}

	const int* elems = tessGetElements ( tess );
	const int nelems = tessGetElementCount ( tess );
	
	for ( int i = 0; i < nelems; ++i ) {
		const int* tri = &elems [ i * 3 ];
		
		if ( this->mVerbose ) {
			MOAIPrint ( "%d: %d, %d, %d\n", i, tri [ 0 ], tri [ 1 ], tri [ 2 ]);
		}
		
		this->mIdxStream.Write < u32 >( base + tri [ 0 ]);
		this->mIdxStream.Write < u32 >( base + tri [ 1 ]);
		this->mIdxStream.Write < u32 >( base + tri [ 2 ]);
	}
	
	if ( this->mVerbose ) {
		MOAIPrint ( "\n" );
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::WriteVertex ( float x, float y, float z, u32 color, u32 vertexExtraID ) {

	this->mVtxStream.Write < float >( x );
	this->mVtxStream.Write < float >( y );
	this->mVtxStream.Write < float >( z );
	this->mVtxStream.Write < u32 >( color );
	
	if ( this->mVtxExtraSize ) {
		vertexExtraID = vertexExtraID % this->mVtxExtras.Size ();
		this->mVtxStream.WriteBytes ( this->mVtxExtras [ vertexExtraID ], this->mVtxExtraSize );
	}
}

//----------------------------------------------------------------//
void MOAIVectorDrawing::WriteVertices ( TESStesselator* tess, float z, u32 color, u32 vertexExtraID ) {

	z = z != 0.0f ? z : this->mDepthOffset;

	if ( this->mVerbose ) {
		MOAIPrint ( "WRITING VERTICES:\n" );
	}

	const float* verts = tessGetVertices ( tess );
	const int nverts = tessGetVertexCount ( tess );
	
	for ( int i = 0; i < nverts; ++i ) {
		
		const ZLVec2D& vert = (( const ZLVec2D* )verts )[ i ];
		
		if ( this->mVerbose ) {
			MOAIPrint ( "%d: %f, %f\n", i, vert.mX, vert.mY );
		}
		this->WriteVertex ( vert.mX, vert.mY, z, color, vertexExtraID );
	}
	
	if ( this->mVerbose ) {
		MOAIPrint ( "\n" );
	}

	this->mDepthOffset += this->mDepthBias;
}