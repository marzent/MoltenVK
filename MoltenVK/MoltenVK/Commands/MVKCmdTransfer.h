/*
 * MVKCmdTransfer.h
 *
 * Copyright (c) 2015-2022 The Brenwill Workshop Ltd. (http://www.brenwill.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "MVKCommand.h"
#include "MVKMTLBufferAllocation.h"
#include "MVKCommandResourceFactory.h"
#include "MVKFoundation.h"
#include "MVKSmallVector.h"

#import <Metal/Metal.h>

class MVKImage;
class MVKBuffer;


#pragma mark -
#pragma mark MVKCmdCopyImage

/**
 * Vulkan command to copy image regions.
 */
class MVKCmdCopyImage : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkImage srcImage,
						VkImageLayout srcImageLayout,
						VkImage dstImage,
						VkImageLayout dstImageLayout,
						uint32_t regionCount,
						const VkImageCopy* pRegions);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkCopyImageInfo2* pImageInfo);

	void encode(MVKCommandEncoder* cmdEncoder) override { encode(cmdEncoder, kMVKCommandUseCopyImage); }

	void encode(MVKCommandEncoder* cmdEncoder, MVKCommandUse commandUse);

protected:
    VkResult validate(MVKCommandBuffer* cmdBuff, const VkImageCopy2* region);

	MVKCommandVector<VkImageCopy2> _vkImageCopies;
	MVKImage* _srcImage;
	MVKImage* _dstImage;
	VkImageLayout _srcLayout;
	VkImageLayout _dstLayout;
};


#pragma mark -
#pragma mark MVKCmdBlitImage

/** Number of vertices in a BLIT rectangle. */
#define kMVKBlitVertexCount		4

/** Combines a VkImageBlit with vertices to render it. */
typedef struct {
	VkImageBlit2 region;
	MVKVertexPosTex vertices[kMVKBlitVertexCount];
} MVKImageBlitRender;

/**
 * Vulkan command to BLIT image regions.
 */
class MVKCmdBlitImage : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkImage srcImage,
						VkImageLayout srcImageLayout,
						VkImage dstImage,
						VkImageLayout dstImageLayout,
						uint32_t regionCount,
						const VkImageBlit* pRegions,
						VkFilter filter);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkBlitImageInfo2* pBlitImageInfo);

	void encode(MVKCommandEncoder* cmdEncoder) override { encode(cmdEncoder, kMVKCommandUseBlitImage); }

	void encode(MVKCommandEncoder* cmdEncoder, MVKCommandUse commandUse);

protected:
	bool canCopyFormats(const VkImageBlit2& region);
	bool canCopy(const VkImageBlit2& region);
	void populateVertices(MVKVertexPosTex* vertices, const VkImageBlit2& region);
    VkResult validate(MVKCommandBuffer* cmdBuff, const VkImageBlit2* region, bool isDestUnwritableLinear);

	MVKCommandVector<VkImageBlit2> _vkImageBlits;
	MVKImage* _srcImage;
	MVKImage* _dstImage;
	VkImageLayout _srcLayout;
	VkImageLayout _dstLayout;
	VkFilter _filter;
};

#pragma mark -
#pragma mark MVKCmdResolveImage

/** Describes Metal texture resolve parameters. */
typedef struct {
    VkImageSubresourceLayers srcSubresource;
    VkImageSubresourceLayers dstSubresource;
} MVKMetalResolveSlice;

/**
 * Vulkan command to resolve image regions.
 */
class MVKCmdResolveImage : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkImage srcImage,
						VkImageLayout srcImageLayout,
						VkImage dstImage,
						VkImageLayout dstImageLayout,
						uint32_t regionCount,
						const VkImageResolve* pRegions);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkResolveImageInfo2* pResolveImageInfo);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
    VkResult validate(MVKCommandBuffer* cmdBuff, const VkImageResolve2* region);

	MVKCommandVector<VkImageResolve2> _vkImageResolves;
    MVKImage* _srcImage;
	MVKImage* _dstImage;
    VkImageLayout _srcLayout;
    VkImageLayout _dstLayout;
};

#pragma mark -
#pragma mark MVKCmdCopyBuffer

/**
 * Vulkan command to copy buffer regions.
 */
class MVKCmdCopyBuffer : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkBuffer srcBuffer,
						VkBuffer destBuffer,
						uint32_t regionCount,
						const VkBufferCopy* pRegions);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkCopyBufferInfo2* pCopyBufferInfo);

	void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
	MVKCommandVector<VkBufferCopy2> _bufferCopyRegions;
	MVKBuffer* _srcBuffer;
	MVKBuffer* _dstBuffer;
};


#pragma mark -
#pragma mark MVKCmdBufferImageCopy

/**
 * Vulkan command to copy either from a buffer to an image, or from an image to a buffer.
 */
class MVKCmdBufferImageCopy : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkBuffer buffer,
						VkImage image,
						VkImageLayout imageLayout,
						uint32_t regionCount,
						const VkBufferImageCopy* pRegions,
						bool toImage);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkCopyBufferToImageInfo2* pBufferToImageInfo);
    VkResult setContent(MVKCommandBuffer* cmdBuff,
                        const VkCopyImageToBufferInfo2* pImageToBufferInfo);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
	bool isArrayTexture();
    VkResult validate(MVKCommandBuffer* cmdBuff);
	MVKCommandVector<VkBufferImageCopy2> _bufferImageCopyRegions;
    MVKBuffer* _buffer;
    MVKImage* _image;
    bool _toImage = false;
};


#pragma mark -
#pragma mark MVKCmdClearAttachments

/**
 * Abstract Vulkan command to clear attachment regions.
 */
class MVKCmdClearAttachments : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						uint32_t attachmentCount,
						const VkClearAttachment* pAttachments,
						uint32_t rectCount,
						const VkClearRect* pRects);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
    uint32_t getVertexCount(MVKCommandEncoder* cmdEncoder);
    void populateVertices(MVKCommandEncoder* cmdEncoder, simd::float4* vertices,
						  float attWidth, float attHeight);
	uint32_t populateVertices(MVKCommandEncoder* cmdEncoder, simd::float4* vertices,
							  uint32_t startVertex, VkClearRect& clearRect,
							  float attWidth, float attHeight);
	virtual VkClearValue& getClearValue(uint32_t attIdx) = 0;
	virtual void setClearValue(uint32_t attIdx, const VkClearValue& clearValue) = 0;

	MVKCommandVector<VkClearRect> _clearRects;
    MVKRPSKeyClearAtt _rpsKey;
	bool _isClearingDepth;
	bool _isClearingStencil;
	float _mtlDepthVal;
    uint32_t _mtlStencilValue;
};


#pragma mark -
#pragma mark MVKCmdClearSingleAttachment

/**
 * Vulkan command to clear regions in a single attachment.
 */
class MVKCmdClearSingleAttachment : public MVKCmdClearAttachments {

protected:
	VkClearValue& getClearValue(uint32_t attIdx) override { return _vkClearValue; }
	void setClearValue(uint32_t attIdx, const VkClearValue& clearValue) override { _vkClearValue = clearValue; }

	VkClearValue _vkClearValue;
};


#pragma mark -
#pragma mark MVKCmdClearMultiAttachments

/**
 * Vulkan command to clear regions multiple attachment.
 */
class MVKCmdClearMultiAttachments : public MVKCmdClearAttachments {

protected:
	VkClearValue& getClearValue(uint32_t attIdx) override { return _vkClearValues[attIdx]; }
	void setClearValue(uint32_t attIdx, const VkClearValue& clearValue) override { _vkClearValues[attIdx] = clearValue; }

	VkClearValue _vkClearValues[kMVKCachedColorAttachmentCount];
};


#pragma mark -
#pragma mark MVKCmdClearImage

/**
 * Abstract Vulkan command to clear an image.
 */
class MVKCmdClearImage : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkImage image,
						VkImageLayout imageLayout,
						const VkClearValue& clearValue,
						uint32_t rangeCount,
						const VkImageSubresourceRange* pRanges);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
    uint32_t populateMetalCopyRegions(const VkImageBlit2* pRegion, uint32_t cpyRgnIdx);
    uint32_t populateMetalBlitRenders(const VkImageBlit2* pRegion, uint32_t rendRgnIdx);
    void populateVertices(MVKVertexPosTex* vertices, const VkImageBlit2* pRegion);
	virtual bool isDepthStencilClear() = 0;

	MVKCommandVector<VkImageSubresourceRange> _subresourceRanges;
	MVKImage* _image;
	VkClearValue _clearValue;
};

#pragma mark -
#pragma mark MVKCmdClearColorImage

/**
 * Abstract Vulkan command to clear a color image.
 */
class MVKCmdClearColorImage : public MVKCmdClearImage {

protected:
	bool isDepthStencilClear() override { return false; }
};


#pragma mark -
#pragma mark MVKCmdClearDepthStencilImage

/**
 * Abstract Vulkan command to clear a depth stencil image.
 */
class MVKCmdClearDepthStencilImage : public MVKCmdClearImage {

protected:
	bool isDepthStencilClear() override { return true; }
};


#pragma mark -
#pragma mark MVKCmdFillBuffer

/** Vulkan command to fill a buffer. */
class MVKCmdFillBuffer : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkBuffer dstBuffer,
						VkDeviceSize dstOffset,
						VkDeviceSize size,
						uint32_t data);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
	MVKBuffer* _dstBuffer;
    VkDeviceSize _dstOffset;
    uint32_t _wordCount;
    uint32_t _dataValue;
};


#pragma mark -
#pragma mark MVKCmdUpdateBuffer

/** Vulkan command to update the contents of a buffer. */
class MVKCmdUpdateBuffer : public MVKCommand {

public:
	VkResult setContent(MVKCommandBuffer* cmdBuff,
						VkBuffer dstBuffer,
						VkDeviceSize dstOffset,
						VkDeviceSize dataSize,
						const void* pData);

    void encode(MVKCommandEncoder* cmdEncoder) override;

protected:
	MVKCommandVector<uint8_t> _srcDataCache;
	MVKBuffer* _dstBuffer;
    VkDeviceSize _dstOffset;
    VkDeviceSize _dataSize;
};
