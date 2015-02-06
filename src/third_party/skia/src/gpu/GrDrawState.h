/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrDrawState_DEFINED
#define GrDrawState_DEFINED

#include "GrBlend.h"
#include "GrColor.h"
#include "GrEffectStage.h"
#include "GrStencil.h"
#include "effects/GrSimpleTextureEffect.h"

#include "SkMatrix.h"

class GrDrawTargetCaps;
class GrPaint;
class GrRenderTarget;
class GrTexture;

class GrDrawState : public SkRefCnt {
public:
    SK_DECLARE_INST_COUNT(GrDrawState)

    GrDrawState() {
        SkDEBUGCODE(fBlockEffectRemovalCnt = 0;)
        this->reset();
    }

    GrDrawState(const SkMatrix& initialViewMatrix) {
        SkDEBUGCODE(fBlockEffectRemovalCnt = 0;)
        this->reset(initialViewMatrix);
    }

    /**
     * Copies another draw state.
     **/
    GrDrawState(const GrDrawState& state) : INHERITED() {
        SkDEBUGCODE(fBlockEffectRemovalCnt = 0;)
        *this = state;
    }

    /**
     * Copies another draw state with a preconcat to the view matrix.
     **/
    GrDrawState(const GrDrawState& state, const SkMatrix& preConcatMatrix);

    virtual ~GrDrawState() { SkASSERT(0 == fBlockEffectRemovalCnt); }

    /**
     * Resets to the default state. GrEffects will be removed from all stages.
     */
    void reset() { this->onReset(NULL); }

    void reset(const SkMatrix& initialViewMatrix) { this->onReset(&initialViewMatrix); }

    /**
     * Initializes the GrDrawState based on a GrPaint, view matrix and render target. Note that
     * GrDrawState encompasses more than GrPaint. Aspects of GrDrawState that have no GrPaint
     * equivalents are set to default values with the exception of vertex attribute state which
     * is unmodified by this function and clipping which will be enabled.
     */
    void setFromPaint(const GrPaint& , const SkMatrix& viewMatrix, GrRenderTarget*);

    ///////////////////////////////////////////////////////////////////////////
    /// @name Vertex Attributes
    ////

    enum {
        kMaxVertexAttribCnt = kLast_GrVertexAttribBinding + 4,
    };

   /**
     * The format of vertices is represented as an array of GrVertexAttribs, with each representing
     * the type of the attribute, its offset, and semantic binding (see GrVertexAttrib in
     * GrTypesPriv.h).
     *
     * The mapping of attributes with kEffect bindings to GrEffect inputs is specified when
     * setEffect is called.
     */

    /**
     *  Sets vertex attributes for next draw. The object driving the templatization
     *  should be a global GrVertexAttrib array that is never changed.
     */
    template <const GrVertexAttrib A[]> void setVertexAttribs(int count) {
        this->setVertexAttribs(A, count);
    }

    const GrVertexAttrib* getVertexAttribs() const { return fVAPtr; }
    int getVertexAttribCount() const { return fVACount; }

    size_t getVertexSize() const;

    /**
     *  Sets default vertex attributes for next draw. The default is a single attribute:
     *  {kVec2f_GrVertexAttribType, 0, kPosition_GrVertexAttribType}
     */
    void setDefaultVertexAttribs();

    /**
     * Getters for index into getVertexAttribs() for particular bindings. -1 is returned if the
     * binding does not appear in the current attribs. These bindings should appear only once in
     * the attrib array.
     */

    int positionAttributeIndex() const {
        return fFixedFunctionVertexAttribIndices[kPosition_GrVertexAttribBinding];
    }
    int localCoordAttributeIndex() const {
        return fFixedFunctionVertexAttribIndices[kLocalCoord_GrVertexAttribBinding];
    }
    int colorVertexAttributeIndex() const {
        return fFixedFunctionVertexAttribIndices[kColor_GrVertexAttribBinding];
    }
    int coverageVertexAttributeIndex() const {
        return fFixedFunctionVertexAttribIndices[kCoverage_GrVertexAttribBinding];
    }

    bool hasLocalCoordAttribute() const {
        return -1 != fFixedFunctionVertexAttribIndices[kLocalCoord_GrVertexAttribBinding];
    }
    bool hasColorVertexAttribute() const {
        return -1 != fFixedFunctionVertexAttribIndices[kColor_GrVertexAttribBinding];
    }
    bool hasCoverageVertexAttribute() const {
        return -1 != fFixedFunctionVertexAttribIndices[kCoverage_GrVertexAttribBinding];
    }

    bool validateVertexAttribs() const;

    /**
     * Helper to save/restore vertex attribs
     */
     class AutoVertexAttribRestore {
     public:
         AutoVertexAttribRestore(GrDrawState* drawState);

         ~AutoVertexAttribRestore() { fDrawState->setVertexAttribs(fVAPtr, fVACount); }

     private:
         GrDrawState*          fDrawState;
         const GrVertexAttrib* fVAPtr;
         int                   fVACount;
     };

    /// @}

    /**
     * Determines whether src alpha is guaranteed to be one for all src pixels
     */
    bool srcAlphaWillBeOne() const;

    /**
     * Determines whether the output coverage is guaranteed to be one for all pixels hit by a draw.
     */
    bool hasSolidCoverage() const;

    /**
     * Depending on features available in the underlying 3D API and the color blend mode requested
     * it may or may not be possible to correctly blend with fractional pixel coverage generated by
     * the fragment shader.
     *
     * This function considers the current draw state and the draw target's capabilities to
     * determine whether coverage can be handled correctly. This function assumes that the caller
     * intends to specify fractional pixel coverage (via setCoverage(), through a coverage vertex
     * attribute, or a coverage effect) but may not have specified it yet.
     */
    bool couldApplyCoverage(const GrDrawTargetCaps& caps) const;

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Color
    ////

    /**
     *  Sets color for next draw to a premultiplied-alpha color.
     *
     *  @param color    the color to set.
     */
    void setColor(GrColor color) {
        fColor = color;
        this->invalidateBlendOptFlags();
    }

    GrColor getColor() const { return fColor; }

    /**
     *  Sets the color to be used for the next draw to be
     *  (r,g,b,a) = (alpha, alpha, alpha, alpha).
     *
     *  @param alpha The alpha value to set as the color.
     */
    void setAlpha(uint8_t a) { this->setColor((a << 24) | (a << 16) | (a << 8) | a); }

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Coverage
    ////

    /**
     * Sets a constant fractional coverage to be applied to the draw. The
     * initial value (after construction or reset()) is 0xff. The constant
     * coverage is ignored when per-vertex coverage is provided.
     */
    void setCoverage(uint8_t coverage) {
        fCoverage = GrColorPackRGBA(coverage, coverage, coverage, coverage);
        this->invalidateBlendOptFlags();
    }

    uint8_t getCoverage() const { return GrColorUnpackR(fCoverage); }

    GrColor getCoverageColor() const { return fCoverage; }

    /// @}

    /**
     * This struct is here so that the GrDrawState can have multiple instances of state information.
     * The use of this will come in a future revision when we want to keep track of the original
     * draw state as well as an optimized version of it.
     */
    struct State {
        State() {
            this->reset();
        }

        State(const GrEffectStage* colorArray, int colorCount,
                      const GrEffectStage* coverageArray, int coverageCount)
            : fColorStages(colorArray, colorCount), fCoverageStages(coverageArray, coverageCount) {
                fSrcBlend = kOne_GrBlendCoeff;
                fDstBlend = kZero_GrBlendCoeff;
        }

        static bool HaveCompatibleState(const State& a, const State& b, bool explicitLocalCoords);

        void reset() {
            fSrcBlend = kOne_GrBlendCoeff;
            fDstBlend = kZero_GrBlendCoeff;
            fColorStages.reset();
            fCoverageStages.reset();
        }

        GrBlendCoeff                        fSrcBlend;
        GrBlendCoeff                        fDstBlend;

        typedef SkSTArray<4, GrEffectStage> EffectStageArray;
        EffectStageArray                    fColorStages;
        EffectStageArray                    fCoverageStages;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @name Effect Stages
    /// Each stage hosts a GrEffect. The effect produces an output color or coverage in the fragment
    /// shader. Its inputs are the output from the previous stage as well as some variables
    /// available to it in the fragment and vertex shader (e.g. the vertex position, the dst color,
    /// the fragment position, local coordinates).
    ///
    /// The stages are divided into two sets, color-computing and coverage-computing. The final
    /// color stage produces the final pixel color. The coverage-computing stages function exactly
    /// as the color-computing but the output of the final coverage stage is treated as a fractional
    /// pixel coverage rather than as input to the src/dst color blend step.
    ///
    /// The input color to the first color-stage is either the constant color or interpolated
    /// per-vertex colors. The input to the first coverage stage is either a constant coverage
    /// (usually full-coverage) or interpolated per-vertex coverage.
    ///
    /// See the documentation of kCoverageDrawing_StateBit for information about disabling the
    /// the color / coverage distinction.
    ////

    const GrEffect* addColorEffect(const GrEffect* effect, int attr0 = -1, int attr1 = -1) {
        SkASSERT(NULL != effect);
        SkNEW_APPEND_TO_TARRAY(&fState.fColorStages, GrEffectStage, (effect, attr0, attr1));
        this->invalidateBlendOptFlags();
        return effect;
    }

    const GrEffect* addCoverageEffect(const GrEffect* effect, int attr0 = -1, int attr1 = -1) {
        SkASSERT(NULL != effect);
        SkNEW_APPEND_TO_TARRAY(&fState.fCoverageStages, GrEffectStage, (effect, attr0, attr1));
        this->invalidateBlendOptFlags();
        return effect;
    }

    /**
     * Creates a GrSimpleTextureEffect that uses local coords as texture coordinates.
     */
    void addColorTextureEffect(GrTexture* texture, const SkMatrix& matrix) {
        this->addColorEffect(GrSimpleTextureEffect::Create(texture, matrix))->unref();
    }

    void addCoverageTextureEffect(GrTexture* texture, const SkMatrix& matrix) {
        this->addCoverageEffect(GrSimpleTextureEffect::Create(texture, matrix))->unref();
    }

    void addColorTextureEffect(GrTexture* texture,
                               const SkMatrix& matrix,
                               const GrTextureParams& params) {
        this->addColorEffect(GrSimpleTextureEffect::Create(texture, matrix, params))->unref();
    }

    void addCoverageTextureEffect(GrTexture* texture,
                                  const SkMatrix& matrix,
                                  const GrTextureParams& params) {
        this->addCoverageEffect(GrSimpleTextureEffect::Create(texture, matrix, params))->unref();
    }

    /**
     * When this object is destroyed it will remove any effects from the draw state that were added
     * after its constructor.
     */
    class AutoRestoreEffects : public ::SkNoncopyable {
    public:
        AutoRestoreEffects() : fDrawState(NULL), fColorEffectCnt(0), fCoverageEffectCnt(0) {}

        AutoRestoreEffects(GrDrawState* ds) : fDrawState(NULL), fColorEffectCnt(0), fCoverageEffectCnt(0) {
            this->set(ds);
        }

        ~AutoRestoreEffects() { this->set(NULL); }

        void set(GrDrawState* ds);

        bool isSet() const { return NULL != fDrawState; }

    private:
        GrDrawState* fDrawState;
        int fColorEffectCnt;
        int fCoverageEffectCnt;
    };

    int numColorStages() const { return fState.fColorStages.count(); }
    int numCoverageStages() const { return fState.fCoverageStages.count(); }
    int numTotalStages() const { return this->numColorStages() + this->numCoverageStages(); }

    const GrEffectStage& getColorStage(int stageIdx) const { return fState.fColorStages[stageIdx]; }
    const GrEffectStage& getCoverageStage(int stageIdx) const { return fState.fCoverageStages[stageIdx]; }

    /**
     * Checks whether any of the effects will read the dst pixel color.
     */
    bool willEffectReadDstColor() const;

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Blending
    ////

    /**
     * Sets the blending function coefficients.
     *
     * The blend function will be:
     *    D' = sat(S*srcCoef + D*dstCoef)
     *
     *   where D is the existing destination color, S is the incoming source
     *   color, and D' is the new destination color that will be written. sat()
     *   is the saturation function.
     *
     * @param srcCoef coefficient applied to the src color.
     * @param dstCoef coefficient applied to the dst color.
     */
    void setBlendFunc(GrBlendCoeff srcCoeff, GrBlendCoeff dstCoeff) {
        fState.fSrcBlend = srcCoeff;
        fState.fDstBlend = dstCoeff;
        this->invalidateBlendOptFlags();
    #ifdef SK_DEBUG
        if (GrBlendCoeffRefsDst(dstCoeff)) {
            GrPrintf("Unexpected dst blend coeff. Won't work correctly with coverage stages.\n");
        }
        if (GrBlendCoeffRefsSrc(srcCoeff)) {
            GrPrintf("Unexpected src blend coeff. Won't work correctly with coverage stages.\n");
        }
    #endif
    }

    GrBlendCoeff getSrcBlendCoeff() const { return fState.fSrcBlend; }
    GrBlendCoeff getDstBlendCoeff() const { return fState.fDstBlend; }

    void getDstBlendCoeff(GrBlendCoeff* srcBlendCoeff,
                          GrBlendCoeff* dstBlendCoeff) const {
        *srcBlendCoeff = fState.fSrcBlend;
        *dstBlendCoeff = fState.fDstBlend;
    }

    /**
     * Sets the blending function constant referenced by the following blending
     * coefficients:
     *      kConstC_GrBlendCoeff
     *      kIConstC_GrBlendCoeff
     *      kConstA_GrBlendCoeff
     *      kIConstA_GrBlendCoeff
     *
     * @param constant the constant to set
     */
    void setBlendConstant(GrColor constant) {
        fBlendConstant = constant;
        this->invalidateBlendOptFlags();
    }

    /**
     * Retrieves the last value set by setBlendConstant()
     * @return the blending constant value
     */
    GrColor getBlendConstant() const { return fBlendConstant; }

    /**
     * Determines whether multiplying the computed per-pixel color by the pixel's fractional
     * coverage before the blend will give the correct final destination color. In general it
     * will not as coverage is applied after blending.
     */
    bool canTweakAlphaForCoverage() const;

    /**
     * Optimizations for blending / coverage to that can be applied based on the current state.
     */
    enum BlendOptFlags {
        /**
         * No optimization
         */
        kNone_BlendOpt                  = 0,
        /**
         * Don't draw at all
         */
        kSkipDraw_BlendOptFlag          = 0x1,
        /**
         * The coverage value does not have to be computed separately from alpha, the the output
         * color can be the modulation of the two.
         */
        kCoverageAsAlpha_BlendOptFlag   = 0x2,
        /**
         * Instead of emitting a src color, emit coverage in the alpha channel and r,g,b are
         * "don't cares".
         */
        kEmitCoverage_BlendOptFlag      = 0x4,
        /**
         * Emit transparent black instead of the src color, no need to compute coverage.
         */
        kEmitTransBlack_BlendOptFlag    = 0x8,
        /**
         * Flag used to invalidate the cached BlendOptFlags, OptSrcCoeff, and OptDstCoeff cached by
         * the get BlendOpts function. 
         */
        kInvalid_BlendOptFlag           = 1 << 31,
    };
    GR_DECL_BITFIELD_OPS_FRIENDS(BlendOptFlags);

    void invalidateBlendOptFlags() {
        fBlendOptFlags = kInvalid_BlendOptFlag;
    }

    /**
     * We don't use suplied vertex color attributes if our blend mode is EmitCoverage or
     * EmitTransBlack
     */
    bool canIgnoreColorAttribute() const;

    /**
     * Determines what optimizations can be applied based on the blend. The coefficients may have
     * to be tweaked in order for the optimization to work. srcCoeff and dstCoeff are optional
     * params that receive the tweaked coefficients. Normally the function looks at the current
     * state to see if coverage is enabled. By setting forceCoverage the caller can speculatively
     * determine the blend optimizations that would be used if there was partial pixel coverage.
     *
     * Subclasses of GrDrawTarget that actually draw (as opposed to those that just buffer for
     * playback) must call this function and respect the flags that replace the output color.
     *
     * If the cached BlendOptFlags does not have the invalidate bit set, then getBlendOpts will
     * simply returned the cached flags and coefficients. Otherwise it will calculate the values. 
     */
    BlendOptFlags getBlendOpts(bool forceCoverage = false,
                               GrBlendCoeff* srcCoeff = NULL,
                               GrBlendCoeff* dstCoeff = NULL) const;

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name View Matrix
    ////

    /**
     * Sets the view matrix to identity and updates any installed effects to compensate for the
     * coord system change.
     */
    bool setIdentityViewMatrix();

    /**
     * Retrieves the current view matrix
     * @return the current view matrix.
     */
    const SkMatrix& getViewMatrix() const { return fViewMatrix; }

    /**
     *  Retrieves the inverse of the current view matrix.
     *
     *  If the current view matrix is invertible, return true, and if matrix
     *  is non-null, copy the inverse into it. If the current view matrix is
     *  non-invertible, return false and ignore the matrix parameter.
     *
     * @param matrix if not null, will receive a copy of the current inverse.
     */
    bool getViewInverse(SkMatrix* matrix) const {
        // TODO: determine whether we really need to leave matrix unmodified
        // at call sites when inversion fails.
        SkMatrix inverse;
        if (fViewMatrix.invert(&inverse)) {
            if (matrix) {
                *matrix = inverse;
            }
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////

    /**
     * Preconcats the current view matrix and restores the previous view matrix in the destructor.
     * Effect matrices are automatically adjusted to compensate and adjusted back in the destructor.
     */
    class AutoViewMatrixRestore : public ::SkNoncopyable {
    public:
        AutoViewMatrixRestore() : fDrawState(NULL) {}

        AutoViewMatrixRestore(GrDrawState* ds, const SkMatrix& preconcatMatrix) {
            fDrawState = NULL;
            this->set(ds, preconcatMatrix);
        }

        ~AutoViewMatrixRestore() { this->restore(); }

        /**
         * Can be called prior to destructor to restore the original matrix.
         */
        void restore();

        void set(GrDrawState* drawState, const SkMatrix& preconcatMatrix);

        /** Sets the draw state's matrix to identity. This can fail because the current view matrix
            is not invertible. */
        bool setIdentity(GrDrawState* drawState);

    private:
        void doEffectCoordChanges(const SkMatrix& coordChangeMatrix);

        GrDrawState*                                        fDrawState;
        SkMatrix                                            fViewMatrix;
        int                                                 fNumColorStages;
        SkAutoSTArray<8, GrEffectStage::SavedCoordChange>   fSavedCoordChanges;
    };

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Render Target
    ////

    /**
     * Sets the render-target used at the next drawing call
     *
     * @param target  The render target to set.
     */
    void setRenderTarget(GrRenderTarget* target) { fRenderTarget.reset(SkSafeRef(target)); }

    /**
     * Retrieves the currently set render-target.
     *
     * @return    The currently set render target.
     */
    const GrRenderTarget* getRenderTarget() const { return fRenderTarget.get(); }
    GrRenderTarget* getRenderTarget() { return fRenderTarget.get(); }

    class AutoRenderTargetRestore : public ::SkNoncopyable {
    public:
        AutoRenderTargetRestore() : fDrawState(NULL), fSavedTarget(NULL) {}
        AutoRenderTargetRestore(GrDrawState* ds, GrRenderTarget* newTarget) {
            fDrawState = NULL;
            fSavedTarget = NULL;
            this->set(ds, newTarget);
        }
        ~AutoRenderTargetRestore() { this->restore(); }

        void restore() {
            if (NULL != fDrawState) {
                fDrawState->setRenderTarget(fSavedTarget);
                fDrawState = NULL;
            }
            SkSafeSetNull(fSavedTarget);
        }

        void set(GrDrawState* ds, GrRenderTarget* newTarget) {
            this->restore();

            if (NULL != ds) {
                SkASSERT(NULL == fSavedTarget);
                fSavedTarget = ds->getRenderTarget();
                SkSafeRef(fSavedTarget);
                ds->setRenderTarget(newTarget);
                fDrawState = ds;
            }
        }
    private:
        GrDrawState* fDrawState;
        GrRenderTarget* fSavedTarget;
    };

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Stencil
    ////

    /**
     * Sets the stencil settings to use for the next draw.
     * Changing the clip has the side-effect of possibly zeroing
     * out the client settable stencil bits. So multipass algorithms
     * using stencil should not change the clip between passes.
     * @param settings  the stencil settings to use.
     */
    void setStencil(const GrStencilSettings& settings) {
        fStencilSettings = settings;
        this->invalidateBlendOptFlags();
    }

    /**
     * Shortcut to disable stencil testing and ops.
     */
    void disableStencil() {
        fStencilSettings.setDisabled();
        this->invalidateBlendOptFlags();
    }

    const GrStencilSettings& getStencil() const { return fStencilSettings; }

    GrStencilSettings* stencil() { return &fStencilSettings; }

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name State Flags
    ////

    /**
     *  Flags that affect rendering. Controlled using enable/disableState(). All
     *  default to disabled.
     */
    enum StateBits {
        /**
         * Perform dithering. TODO: Re-evaluate whether we need this bit
         */
        kDither_StateBit        = 0x01,
        /**
         * Perform HW anti-aliasing. This means either HW FSAA, if supported by the render target,
         * or smooth-line rendering if a line primitive is drawn and line smoothing is supported by
         * the 3D API.
         */
        kHWAntialias_StateBit   = 0x02,
        /**
         * Draws will respect the clip, otherwise the clip is ignored.
         */
        kClip_StateBit          = 0x04,
        /**
         * Disables writing to the color buffer. Useful when performing stencil
         * operations.
         */
        kNoColorWrites_StateBit = 0x08,

        /**
         * Usually coverage is applied after color blending. The color is blended using the coeffs
         * specified by setBlendFunc(). The blended color is then combined with dst using coeffs
         * of src_coverage, 1-src_coverage. Sometimes we are explicitly drawing a coverage mask. In
         * this case there is no distinction between coverage and color and the caller needs direct
         * control over the blend coeffs. When set, there will be a single blend step controlled by
         * setBlendFunc() which will use coverage*color as the src color.
         */
         kCoverageDrawing_StateBit = 0x10,

        // Users of the class may add additional bits to the vector
        kDummyStateBit,
        kLastPublicStateBit = kDummyStateBit-1,
    };

    void resetStateFlags() {
        fFlagBits = 0;
        this->invalidateBlendOptFlags();
    }

    /**
     * Enable render state settings.
     *
     * @param stateBits bitfield of StateBits specifying the states to enable
     */
    void enableState(uint32_t stateBits) {
        fFlagBits |= stateBits;
        this->invalidateBlendOptFlags();
    }

    /**
     * Disable render state settings.
     *
     * @param stateBits bitfield of StateBits specifying the states to disable
     */
    void disableState(uint32_t stateBits) {
        fFlagBits &= ~(stateBits);
        this->invalidateBlendOptFlags();
    }

    /**
     * Enable or disable stateBits based on a boolean.
     *
     * @param stateBits bitfield of StateBits to enable or disable
     * @param enable    if true enable stateBits, otherwise disable
     */
    void setState(uint32_t stateBits, bool enable) {
        if (enable) {
            this->enableState(stateBits);
        } else {
            this->disableState(stateBits);
        }
    }

    bool isStateFlagEnabled(uint32_t stateBit) const { return 0 != (stateBit & fFlagBits); }

    bool isDitherState() const { return 0 != (fFlagBits & kDither_StateBit); }
    bool isHWAntialiasState() const { return 0 != (fFlagBits & kHWAntialias_StateBit); }
    bool isClipState() const { return 0 != (fFlagBits & kClip_StateBit); }
    bool isColorWriteDisabled() const { return 0 != (fFlagBits & kNoColorWrites_StateBit); }
    bool isCoverageDrawing() const { return 0 != (fFlagBits & kCoverageDrawing_StateBit); }

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Face Culling
    ////

    enum DrawFace {
        kInvalid_DrawFace = -1,

        kBoth_DrawFace,
        kCCW_DrawFace,
        kCW_DrawFace,
    };

    /**
     * Controls whether clockwise, counterclockwise, or both faces are drawn.
     * @param face  the face(s) to draw.
     */
    void setDrawFace(DrawFace face) {
        SkASSERT(kInvalid_DrawFace != face);
        fDrawFace = face;
    }

    /**
     * Gets whether the target is drawing clockwise, counterclockwise,
     * or both faces.
     * @return the current draw face(s).
     */
    DrawFace getDrawFace() const { return fDrawFace; }

    /// @}

    ///////////////////////////////////////////////////////////////////////////
    /// @name Hints
    /// Hints that when provided can enable optimizations.
    ////

    enum Hints { kVertexColorsAreOpaque_Hint = 0x1, };

    void setHint(Hints hint, bool value) { fHints = value ? (fHints | hint) : (fHints & ~hint); }

    /// @}

    ///////////////////////////////////////////////////////////////////////////

    /** Return type for CombineIfPossible. */
    enum CombinedState {
        /** The GrDrawStates cannot be combined. */
        kIncompatible_CombinedState,
        /** Either draw state can be used in place of the other. */
        kAOrB_CombinedState,
        /** Use the first draw state. */
        kA_CombinedState,
        /** Use the second draw state. */
        kB_CombinedState,
    };

    /** This function determines whether the GrDrawStates used for two draws can be combined into
        a single GrDrawState. This is used to avoid storing redundant GrDrawStates and to determine
        if draws can be batched. The return value indicates whether combining is possible and, if
        so, which of the two inputs should be used. */
    static CombinedState CombineIfPossible(const GrDrawState& a, const GrDrawState& b,
                                           const GrDrawTargetCaps& caps);

    GrDrawState& operator= (const GrDrawState& that);

private:
    void onReset(const SkMatrix* initialViewMatrix);

    BlendOptFlags calcBlendOpts(bool forceCoverage = false,
                                GrBlendCoeff* srcCoeff = NULL,
                                GrBlendCoeff* dstCoeff = NULL) const;

    // These fields are roughly sorted by decreasing likelihood of being different in op==
    SkAutoTUnref<GrRenderTarget>        fRenderTarget;
    GrColor                             fColor;
    SkMatrix                            fViewMatrix;
    GrColor                             fBlendConstant;
    uint32_t                            fFlagBits;
    const GrVertexAttrib*               fVAPtr;
    int                                 fVACount;
    GrStencilSettings                   fStencilSettings;
    GrColor                             fCoverage;
    DrawFace                            fDrawFace;

    State                               fState;

    uint32_t                            fHints;
    
    mutable GrBlendCoeff                fOptSrcBlend;
    mutable GrBlendCoeff                fOptDstBlend;
    mutable BlendOptFlags               fBlendOptFlags;

    // This is simply a different representation of info in fVertexAttribs and thus does
    // not need to be compared in op==.
    int fFixedFunctionVertexAttribIndices[kGrFixedFunctionVertexAttribBindingCnt];

    // Some of the auto restore objects assume that no effects are removed during their lifetime.
    // This is used to assert that this condition holds.
    SkDEBUGCODE(int fBlockEffectRemovalCnt;)

    /**
     *  Sets vertex attributes for next draw.
     *
     *  @param attribs    the array of vertex attributes to set.
     *  @param count      the number of attributes being set, limited to kMaxVertexAttribCnt.
     */
    void setVertexAttribs(const GrVertexAttrib attribs[], int count);

    typedef SkRefCnt INHERITED;
};

GR_MAKE_BITFIELD_OPS(GrDrawState::BlendOptFlags);

#endif