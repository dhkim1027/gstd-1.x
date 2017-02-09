/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2015 RidgeRun Engineering <support@ridgerun.com>
 *
 * This file is part of Gstd.
 *
 * Gstd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gstd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Gstd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GSTD_IDELETER_H__
#define __GSTD_IDELETER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GSTD_TYPE_IDELETER                (gstd_ideleter_get_type ())
#define GSTD_IDELETER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSTD_TYPE_IDELETER, GstdIDeleter))
#define GSTD_IS_IDELETER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSTD_TYPE_IDELETER))
#define GSTD_IDELETER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GSTD_TYPE_IDELETER, GstdIDeleterInterface))

typedef struct _GstdIDeleter GstdIDeleter;
typedef struct _GstdIDeleterInterface GstdIDeleterInterface;

// Avoid cyclic dependecies by forward declaration
typedef struct _GstdObject GstdObject;

struct _GstdIDeleterInterface {
  GTypeInterface parent;

  void (* delete) (GstdIDeleter *self, GstdObject *object);
};

GType gstd_ideleter_get_type (void);

void gstd_ideleter_delete (GstdIDeleter *self, GstdObject *object);

G_END_DECLS

#endif // __GSTD_IDELETER_H__