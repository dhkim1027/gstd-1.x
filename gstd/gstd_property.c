/*
 * Gstreamer Daemon - Gst Launch under steroids
 * Copyright (C) 2017 RidgeRun Engineering <support@ridgerun.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_property.h"

enum {
    PROP_TARGET = 1,
    N_PROPERTIES
};

#define DEFAULT_PROP_TARGET NULL

/* Gstd Property debugging category */
GST_DEBUG_CATEGORY_STATIC(gstd_property_debug);
#define GST_CAT_DEFAULT gstd_property_debug

#define GSTD_DEBUG_DEFAULT_LEVEL GST_LEVEL_INFO

/**
 * GstdProperty:
 * A wrapper for the conventional property
 */

G_DEFINE_TYPE (GstdProperty, gstd_property, GSTD_TYPE_OBJECT)

/* VTable */
static void
gstd_property_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
gstd_property_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
gstd_property_dispose (GObject *);
static GstdReturnCode
gstd_property_to_string (GstdObject * obj, gchar ** outstring);
static void
gstd_property_add_value_default (GstdProperty * self, GstdIFormatter * formatter,
    GValue * value);

static void
gstd_property_class_init (GstdPropertyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstdObjectClass * gstdc = GSTD_OBJECT_CLASS (klass);
  GParamSpec *properties[N_PROPERTIES] = { NULL, };
  guint debug_color;

  object_class->set_property = gstd_property_set_property;
  object_class->get_property = gstd_property_get_property;
  object_class->dispose = gstd_property_dispose;

  properties[PROP_TARGET] =
    g_param_spec_object ("target",
			 "Target",
			 "The target object owning the property",
			 G_TYPE_OBJECT,
			 G_PARAM_READWRITE |
			 G_PARAM_CONSTRUCT_ONLY |
			 G_PARAM_STATIC_STRINGS |
			 GSTD_PARAM_READ);


  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     properties);

  gstdc->to_string = GST_DEBUG_FUNCPTR(gstd_property_to_string);

  klass->add_value = GST_DEBUG_FUNCPTR(gstd_property_add_value_default);

  /* Initialize debug category with nice colors */
  debug_color = GST_DEBUG_FG_BLACK | GST_DEBUG_BOLD | GST_DEBUG_BG_WHITE;
  GST_DEBUG_CATEGORY_INIT (gstd_property_debug, "gstdproperty", debug_color,
			   "Gstd Property category");

}

static void
gstd_property_init (GstdProperty *self)
{
  GST_INFO_OBJECT(self, "Initializing property");
  self->target = DEFAULT_PROP_TARGET;
}

static void
gstd_property_dispose (GObject *object)
{
  GstdProperty *self = GSTD_PROPERTY(object);

  GST_INFO_OBJECT(self, "Disposing %s property", GSTD_OBJECT_NAME(self));

  if (self->target) {
    g_object_unref(self->target);
    self->target = NULL;
  }

  G_OBJECT_CLASS(gstd_property_parent_class)->dispose(object);
}

static void
gstd_property_get_property (GObject        *object,
			    guint           property_id,
			    GValue         *value,
			    GParamSpec     *pspec)
{
  GstdProperty *self = GSTD_PROPERTY(object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);

  switch (property_id) {
  case PROP_TARGET:
    GST_DEBUG_OBJECT(self, "Returning property owner %p (%s)", self->target, GST_OBJECT_NAME(self->target));
    g_value_set_object (value, self->target);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static void
gstd_property_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  GstdProperty *self = GSTD_PROPERTY (object);

  gstd_object_set_code (GSTD_OBJECT(self), GSTD_EOK);

  switch (property_id) {
  case PROP_TARGET:
    if (self->target)
      g_object_unref (self->target);
    self->target = g_value_dup_object (value);
    GST_DEBUG_OBJECT(self, "Setting property owner %p (%s)",self->target,
    		     GST_OBJECT_NAME(self->target));
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    gstd_object_set_code (GSTD_OBJECT(self), GSTD_NO_RESOURCE);
    break;
  }
}

static GstdReturnCode
gstd_property_to_string (GstdObject * obj, gchar ** outstring)
{
  GParamSpec * property;
  GstdProperty * self;
  GstdPropertyClass * klass;
  GValue value = G_VALUE_INIT;
  gchar *sflags;
  const gchar *typename;

  g_return_val_if_fail (GSTD_IS_OBJECT (obj), GSTD_NULL_ARGUMENT);
  g_return_val_if_fail (outstring, GSTD_NULL_ARGUMENT);

  self = GSTD_PROPERTY(obj);
  klass = GSTD_PROPERTY_GET_CLASS(self);

  property = g_object_class_find_property(G_OBJECT_GET_CLASS(self->target),
      GSTD_OBJECT_NAME(self));

  /* Describe each parameter using a structure */
  gstd_iformatter_begin_object (obj->formatter);

  gstd_iformatter_set_member_name (obj->formatter,"name");
  gstd_iformatter_set_string_value (obj->formatter, property->name);

  gstd_iformatter_set_member_name (obj->formatter,"value");

  g_value_init (&value, property->value_type);
  g_object_get_property (G_OBJECT(self->target), property->name, &value);

  g_assert (klass->add_value);
  klass->add_value (self, obj->formatter, &value);

  g_value_unset (&value);

  gstd_iformatter_set_member_name (obj->formatter, "param_spec");
  /* Describe the parameter specs using a structure */
  gstd_iformatter_begin_object (obj->formatter);

  gstd_iformatter_set_member_name (obj->formatter, "blurb");
  gstd_iformatter_set_string_value (obj->formatter,property->_blurb);

  typename = g_type_name (property->value_type);
  gstd_iformatter_set_member_name (obj->formatter, "type");
  gstd_iformatter_set_string_value (obj->formatter,typename);

  g_value_init (&value, GSTD_TYPE_PARAM_FLAGS);
  g_value_set_flags (&value, property->flags);
  sflags = g_strdup_value_contents(&value);
  g_value_unset (&value);

  gstd_iformatter_set_member_name (obj->formatter, "access");
  gstd_iformatter_set_string_value (obj->formatter, sflags);

  g_free (sflags);

  gstd_iformatter_set_member_name (obj->formatter, "construct");
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, GSTD_PARAM_IS_DELETE(property->flags));
  gstd_iformatter_set_value (obj->formatter, &value);
  g_value_unset (&value);

  /* Close parameter specs structure */
  gstd_iformatter_end_object (obj->formatter);

  /* Close parameter structure */
  gstd_iformatter_end_object (obj->formatter);

  gstd_iformatter_generate (obj->formatter, outstring);

  return GSTD_EOK;
}

static void
gstd_property_add_value_default (GstdProperty * self, GstdIFormatter * formatter,
    GValue * value)
{
  gchar * svalue;

  g_return_if_fail (self);
  g_return_if_fail (formatter);
  g_return_if_fail (value);

  svalue = g_strdup_value_contents (value);
  gstd_iformatter_set_string_value (formatter, svalue);
  g_free (svalue);
}
