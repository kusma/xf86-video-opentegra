/*
 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author: Alan Hourihane <alanh@tungstengraphics.com>
 *
 */

#include <errno.h>
#include <drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "shadow.h"
#include "exa.h"

#define DRV_ERROR(msg)	xf86DrvMsg(pScrn->scrnIndex, X_ERROR, msg);

typedef struct {
   int            lastInstance;
   int            refCount;
   ScrnInfoPtr    pScrn_1;
   ScrnInfoPtr    pScrn_2;
} EntRec, *EntPtr;

typedef struct _modesettingRec {
   int fd;
   unsigned int fb_id;
   void *virtual;
   drmBO bo;

   EntPtr entityPrivate;	

   void (*PointerMoved)(int, int, int);

   int Chipset;
   EntityInfoPtr pEnt;
#if XSERVER_LIBPCIACCESS
   struct pci_device *PciInfo;
#else
   pciVideoPtr PciInfo;
   PCITAG PciTag;
#endif

   Bool noAccel;
   Bool SWCursor;
   CloseScreenProcPtr CloseScreen;

   Bool directRenderingDisabled;	/* DRI disabled in PreInit. */
   Bool directRenderingEnabled;		/* DRI enabled this generation. */

   /* Broken-out options. */
   OptionInfoPtr Options;

   unsigned int SaveGeneration;

   /* shadowfb */
   CARD8 *shadowMem;
   Bool shadowFB;
   CreateScreenResourcesProcPtr createScreenResources;
   ShadowUpdateProc update;

   /* exa */
   ExaDriverPtr pExa;
   drmBO exa_bo;
} modesettingRec, *modesettingPtr;

#define modesettingPTR(p) ((modesettingPtr)((p)->driverPrivate))