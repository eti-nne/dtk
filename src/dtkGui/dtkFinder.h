/* dtkFinder.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sat Jun 12 15:45:12 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Sep 15 15:14:42 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 28
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKFINDER_H
#define DTKFINDER_H

#include <QtCore>
#include <QtGui>

#include "dtkGuiExport.h"

// /////////////////////////////////////////////////////////////////
// dtkFinderToolBar
// /////////////////////////////////////////////////////////////////

class dtkFinderToolBarPrivate;

class DTKGUI_EXPORT dtkFinderToolBar : public QToolBar
{
    Q_OBJECT

public:
     dtkFinderToolBar(QWidget *parent = 0);
    ~dtkFinderToolBar(void);

     QSize sizeHint (void) const;

signals:
    void changed(const QString& path);
    void listView (void);
    void treeView (void);
    void showHiddenFiles(bool);

public slots:
    void setPath(const QString& path);
    void onPrev (void);
    void onNext (void);

protected:

private:
    dtkFinderToolBarPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkFinderSideView
// /////////////////////////////////////////////////////////////////

class dtkFinderSideViewPrivate;

class DTKGUI_EXPORT dtkFinderSideView : public QTreeWidget
{
    Q_OBJECT

    Q_PROPERTY(int   headerFontSize
               READ  headerFontSize
               WRITE setHeaderFontSize)

public:
     dtkFinderSideView(QWidget *parent = 0);
    ~dtkFinderSideView(void);

    void populate(void);

    int headerFontSize(void) const;

    QSize sizeHint (void) const;

signals:
    void changed(const QString& path);

public slots:
    void setPath(const QString& path);
    void addBookmark(const QString& path);
    void clearBookmarks(void);    
    void setHeaderFontSize(int value);

private slots:
    void onItemCicked(QTreeWidgetItem *, int);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    QString driveLabel(QString drive);

private:
    dtkFinderSideViewPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkFinderPathBar
// /////////////////////////////////////////////////////////////////

class dtkFinderPathBarPrivate;

class DTKGUI_EXPORT dtkFinderPathBar : public QFrame
{
    Q_OBJECT

public:
     dtkFinderPathBar(QWidget *parent = 0);
    ~dtkFinderPathBar(void);

     QSize sizeHint (void) const;

signals:
    void changed(const QString& path);

public slots:
    void setPath(const QString &path);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    dtkFinderPathBarPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkFinderListView
// /////////////////////////////////////////////////////////////////

class dtkFinderListViewPrivate;

class DTKGUI_EXPORT dtkFinderListView : public QListView
{
    Q_OBJECT

public:
     dtkFinderListView(QWidget *parent = 0);
    ~dtkFinderListView(void);

    void addContextMenuAction(QAction *action);

    QString selectedPath(void) const;

    void allowFileBookmarking(bool isAllowed);

signals:
    void changed(const QString& path);
    void bookmarked(const QString& path);

public slots:
    void onBookmarkSelectedItemRequested(void);

protected slots:
    void updateContextMenu(const QPoint&);

protected:
     void keyPressEvent(QKeyEvent *event);
     void mouseDoubleClickEvent(QMouseEvent *event);
     void startDrag(Qt::DropActions supportedActions);

private:
     dtkFinderListViewPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkFinderTreeView
// /////////////////////////////////////////////////////////////////

class dtkFinderTreeViewPrivate;

class DTKGUI_EXPORT dtkFinderTreeView : public QTreeView
{
    Q_OBJECT

public:
     dtkFinderTreeView(QWidget *parent = 0);
    ~dtkFinderTreeView(void);

    int	sizeHintForColumn(int column) const;

    void addContextMenuAction(QAction *action);

    QString selectedPath(void) const;

    void allowFileBookmarking(bool isAllowed);

signals:
    void changed(const QString& path);
    void bookmarked(const QString& path);

public slots:
    void onBookmarkSelectedItemRequested(void);

protected slots:
    void updateContextMenu(const QPoint&);

protected:
     void keyPressEvent(QKeyEvent *event);
     void mouseDoubleClickEvent(QMouseEvent *event);
     void startDrag(Qt::DropActions supportedActions);
     void resizeEvent(QResizeEvent * event);

private:
     dtkFinderTreeViewPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// dtkFinder
// /////////////////////////////////////////////////////////////////

class dtkFinderPrivate;

class DTKGUI_EXPORT dtkFinder : public QWidget
{
    Q_OBJECT

public:
     dtkFinder(QWidget *parent = 0);
    ~dtkFinder(void);

    void addContextMenuAction(QAction *action);

    QString selectedPath(void) const;

    void allowFileBookmarking(bool isAllowed);

signals:
    void changed(const QString& path);
    void bookmarked(const QString& path);
    void fileDoubleClicked(const QString &filename);
    void itemSelected(const QString& path);

public slots:
    void setPath(const QString& path);
    void switchToListView(void);
    void switchToTreeView(void);
    void onShowHiddenFiles(bool);

    /** Bookmarks the currently selected item. */
    void onBookmarkSelectedItemRequested(void);

protected slots:
    void onIndexDoubleClicked(QModelIndex);
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    dtkFinderPrivate *d;
};

#endif // DTKFINDER_H
