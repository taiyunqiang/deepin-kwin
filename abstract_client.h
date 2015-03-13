/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2015 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#ifndef KWIN_ABSTRACT_CLIENT_H
#define KWIN_ABSTRACT_CLIENT_H

#include "toplevel.h"
#include "options.h"
#include "rules.h"

namespace KWin
{

class TabGroup;

namespace TabBox
{
class TabBoxClientImpl;
}

class AbstractClient : public Toplevel
{
    Q_OBJECT
    /**
     * Whether this Client is the currently visible Client in its Client Group (Window Tabs).
     * For change connect to the visibleChanged signal on the Client's Group.
     **/
    Q_PROPERTY(bool isCurrentTab READ isCurrentTab)
    /**
     * Whether this Client is active or not. Use Workspace::activateClient() to activate a Client.
     * @see Workspace::activateClient
     **/
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    /**
     * Whether the Client should be excluded from window switching effects.
     **/
    Q_PROPERTY(bool skipSwitcher READ skipSwitcher WRITE setSkipSwitcher NOTIFY skipSwitcherChanged)
    /**
     * Whether the window can be closed by the user. The value is evaluated each time the getter is called.
     * Because of that no changed signal is provided.
     **/
    Q_PROPERTY(bool closeable READ isCloseable)
    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
public:
    virtual ~AbstractClient();

    QWeakPointer<TabBox::TabBoxClientImpl> tabBoxClient() const {
        return m_tabBoxClient.toWeakRef();
    }
    bool isFirstInTabBox() const {
        return m_firstInTabBox;
    }
    bool skipSwitcher() const {
        return m_skipSwitcher;
    }
    void setSkipSwitcher(bool set);

    const QIcon &icon() const {
        return m_icon;
    }

    bool isActive() const {
        return m_active;
    }
    /**
     * Sets the client's active state to \a act.
     *
     * This function does only change the visual appearance of the client,
     * it does not change the focus setting. Use
     * Workspace::activateClient() or Workspace::requestFocus() instead.
     *
     * If a client receives or looses the focus, it calls setActive() on
     * its own.
     **/
    void setActive(bool);

    virtual void updateMouseGrab();
    virtual QString caption(bool full = true, bool stripped = false) const = 0;
    virtual bool isMinimized() const = 0;
    virtual bool isCloseable() const = 0;
    // TODO: remove boolean trap
    virtual bool isShown(bool shaded_is_shown) const = 0;
    virtual bool wantsTabFocus() const = 0;
    virtual bool isFullScreen() const = 0;
    // TODO: remove boolean trap
    virtual AbstractClient *findModal(bool allow_itself = false) = 0;
    virtual void cancelAutoRaise() = 0;
    virtual bool isTransient() const;
    /**
     * Returns true for "special" windows and false for windows which are "normal"
     * (normal=window which has a border, can be moved by the user, can be closed, etc.)
     * true for Desktop, Dock, Splash, Override and TopMenu (and Toolbar??? - for now)
     * false for Normal, Dialog, Utility and Menu (and Toolbar??? - not yet) TODO
     */
    virtual bool isSpecialWindow() const = 0;
    virtual void sendToScreen(int screen) = 0;
    virtual const QKeySequence &shortcut() const  = 0;
    virtual void setShortcut(const QString &cut) = 0;
    virtual bool performMouseCommand(Options::MouseCommand, const QPoint &globalPos) = 0;
    virtual void setOnAllDesktops(bool set) = 0;
    virtual void setDesktop(int) = 0;
    virtual void minimize(bool avoid_animation = false) = 0;
    virtual void unminimize(bool avoid_animation = false)= 0;
    virtual void setFullScreen(bool set, bool user = true) = 0;
    virtual bool keepAbove() const = 0;
    virtual void setKeepAbove(bool) = 0;
    virtual bool keepBelow() const = 0;
    virtual void setKeepBelow(bool) = 0;
    virtual TabGroup *tabGroup() const;
    Q_INVOKABLE virtual bool untab(const QRect &toGeometry = QRect(), bool clientRemoved = false);
    virtual bool isCurrentTab() const;
    virtual MaximizeMode maximizeMode() const = 0;
    virtual void maximize(MaximizeMode) = 0;
    virtual bool noBorder() const = 0;
    virtual void setNoBorder(bool set) = 0;
    virtual void blockActivityUpdates(bool b = true) = 0;
    virtual QPalette palette() const = 0;
    virtual bool isResizable() const = 0;
    virtual bool isMovable() const = 0;
    virtual bool isMovableAcrossScreens() const = 0;
    virtual bool isShade() const = 0; // True only for ShadeNormal
    virtual ShadeMode shadeMode() const = 0; // Prefer isShade()
    virtual void setShade(bool set) = 0;
    virtual void setShade(ShadeMode mode) = 0;
    virtual bool isShadeable() const = 0;
    virtual bool isMaximizable() const = 0;
    virtual bool isMinimizable() const = 0;
    virtual bool userCanSetFullScreen() const = 0;
    virtual bool userCanSetNoBorder() const = 0;
    virtual void setOnAllActivities(bool set) = 0;
    virtual const WindowRules* rules() const = 0;
    virtual void takeFocus() = 0;
    virtual bool wantsInput() const = 0;
    virtual void checkWorkspacePosition(QRect oldGeometry = QRect(), int oldDesktop = -2) = 0;
    virtual xcb_timestamp_t userTime() const;
    virtual void demandAttention(bool set = true) = 0;
    virtual void updateWindowRules(Rules::Types selection) = 0;

    virtual void growHorizontal();
    virtual void shrinkHorizontal();
    virtual void growVertical();
    virtual void shrinkVertical();

    /**
     * These values represent positions inside an area
     */
    enum Position {
        // without prefix, they'd conflict with Qt::TopLeftCorner etc. :(
        PositionCenter         = 0x00,
        PositionLeft           = 0x01,
        PositionRight          = 0x02,
        PositionTop            = 0x04,
        PositionBottom         = 0x08,
        PositionTopLeft        = PositionLeft | PositionTop,
        PositionTopRight       = PositionRight | PositionTop,
        PositionBottomLeft     = PositionLeft | PositionBottom,
        PositionBottomRight    = PositionRight | PositionBottom
    };
    virtual Position titlebarPosition() const = 0;

    // a helper for the workspace window packing. tests for screen validity and updates since in maximization case as with normal moving
    virtual void packTo(int left, int top);

    enum QuickTileFlag {
        QuickTileNone = 0,
        QuickTileLeft = 1,
        QuickTileRight = 1<<1,
        QuickTileTop = 1<<2,
        QuickTileBottom = 1<<3,
        QuickTileHorizontal = QuickTileLeft|QuickTileRight,
        QuickTileVertical = QuickTileTop|QuickTileBottom,
        QuickTileMaximize = QuickTileLeft|QuickTileRight|QuickTileTop|QuickTileBottom
    };
    Q_DECLARE_FLAGS(QuickTileMode, QuickTileFlag)
    /** Set the quick tile mode ("snap") of this window.
     * This will also handle preserving and restoring of window geometry as necessary.
     * @param mode The tile mode (left/right) to give this window.
     */
    virtual void setQuickTileMode(QuickTileMode mode, bool keyboard = false) = 0;
    virtual void updateLayer();

    // TODO: remove boolean trap
    static bool belongToSameApplication(const AbstractClient* c1, const AbstractClient* c2, bool active_hack = false);

public Q_SLOTS:
    virtual void closeWindow() = 0;

Q_SIGNALS:
    void skipSwitcherChanged();
    void iconChanged();
    void activeChanged();

protected:
    AbstractClient();
    void setFirstInTabBox(bool enable) {
        m_firstInTabBox = enable;
    }
    void setIcon(const QIcon &icon);
    /**
     * Called from ::setActive once the active value got updated, but before the changed signal
     * is emitted.
     *
     * Default implementation does nothing.
     **/
    virtual void doSetActive();
    // TODO: remove boolean trap
    virtual bool belongsToSameApplication(const AbstractClient *other, bool active_hack) const = 0;

private:
    QSharedPointer<TabBox::TabBoxClientImpl> m_tabBoxClient;
    bool m_firstInTabBox = false;
    bool m_skipSwitcher = false;
    QIcon m_icon;
    bool m_active = false;
};

}

Q_DECLARE_METATYPE(KWin::AbstractClient*)
Q_DECLARE_METATYPE(QList<KWin::AbstractClient*>)
Q_DECLARE_OPERATORS_FOR_FLAGS(KWin::AbstractClient::QuickTileMode)

#endif
