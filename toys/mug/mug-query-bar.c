/*
** Copyright (C) 2011 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 3, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation,
** Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**
*/


#include "mug-query-bar.h"
/* include other impl specific header files */

/* 'private'/'protected' functions */
static void mug_query_bar_class_init (MugQueryBarClass * klass);
static void mug_query_bar_init (MugQueryBar * obj);
static void mug_query_bar_finalize (GObject * obj);

/* list my signals  */
enum {
	MUG_QUERY_CHANGED,
	LAST_SIGNAL
};

typedef struct _MugQueryBarPrivate MugQueryBarPrivate;
struct _MugQueryBarPrivate {
	GtkWidget *_entry;
};
#define MUG_QUERY_BAR_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                           MUG_TYPE_QUERY_BAR, \
                                           MugQueryBarPrivate))
/* globals */
static GtkContainerClass *parent_class = NULL;

static guint signals[LAST_SIGNAL] = { 0 };

GType
mug_query_bar_get_type (void)
{
	static GType my_type = 0;
	if (!my_type) {
		static const GTypeInfo my_info = {
			sizeof (MugQueryBarClass),
			NULL,	/* base init */
			NULL,	/* base finalize */
			(GClassInitFunc) mug_query_bar_class_init,
			NULL,	/* class finalize */
			NULL,	/* class data */
			sizeof (MugQueryBar),
			0,	/* n_preallocs, ignored since 2.10 */
			(GInstanceInitFunc) mug_query_bar_init,
			NULL
		};
		my_type = g_type_register_static (GTK_TYPE_HBOX,
						  "MugQueryBar", &my_info, 0);
	}
	return my_type;
}

static void
mug_query_bar_class_init (MugQueryBarClass * klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass *) klass;

	parent_class = g_type_class_peek_parent (klass);
	gobject_class->finalize = mug_query_bar_finalize;

	g_type_class_add_private (gobject_class, sizeof (MugQueryBarPrivate));

	/* signal definitions go here, e.g.: */
	signals[MUG_QUERY_CHANGED] =
	    g_signal_new ("query_changed",
			  G_TYPE_FROM_CLASS (gobject_class),
			  G_SIGNAL_RUN_FIRST,
			  G_STRUCT_OFFSET (MugQueryBarClass, query_changed),
			  NULL, NULL,
			  g_cclosure_marshal_VOID__STRING,
			  G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
on_entry_activated (GtkWidget * w, MugQueryBar * bar)
{
	g_signal_emit (G_OBJECT (bar), signals[MUG_QUERY_CHANGED], 0,
		       gtk_entry_get_text (GTK_ENTRY (w)));
}

static void
mug_query_bar_init (MugQueryBar * obj)
{
	MugQueryBarPrivate *priv;

	priv = MUG_QUERY_BAR_GET_PRIVATE (obj);

	priv->_entry = gtk_entry_new ();

	g_signal_connect (priv->_entry, "activate",
			  G_CALLBACK (on_entry_activated), obj);

	gtk_box_pack_start (GTK_BOX (obj), priv->_entry, TRUE, TRUE, 0);
}

static void
mug_query_bar_finalize (GObject * obj)
{
/* 	free/unref instance resources here */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

GtkWidget *
mug_query_bar_new (void)
{
	return GTK_WIDGET (g_object_new (MUG_TYPE_QUERY_BAR, NULL));
}

void
mug_query_bar_set_query (MugQueryBar * self, const char *query, gboolean run)
{
	MugQueryBarPrivate *priv;

	g_return_if_fail (MUG_IS_QUERY_BAR (self));
	priv = MUG_QUERY_BAR_GET_PRIVATE (self);

	gtk_entry_set_text (GTK_ENTRY (priv->_entry), query ? query : "");

	if (run)
		on_entry_activated (priv->_entry, self);
}

void
mug_query_bar_grab_focus (MugQueryBar * self)
{
	g_return_if_fail (MUG_IS_QUERY_BAR (self));

	gtk_widget_grab_focus
	    (GTK_WIDGET (MUG_QUERY_BAR_GET_PRIVATE (self)->_entry));
}
