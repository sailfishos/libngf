#!/usr/bin/python

import os, sys
import gobject
import dbus, dbus.service
from dbus.mainloop.glib import DBusGMainLoop

SERVER_DBUS_NAME = 'com.nokia.NonGraphicFeedback1'
SERVER_DBUS_PATH = '/com/nokia/NonGraphicFeedback1'
SERVER_DBUS_IFACE = 'com.nokia.NonGraphicFeedback1'

class DummyServer (dbus.service.Object):
    def __init__ (self, *kw, **kwargs):
        self.id_count = 0
        self.events = {}

        dbus.service.Object.__init__ (self, dbus.SessionBus (), SERVER_DBUS_PATH)
        print self._request_name ()

    def _request_name (self):
        bus = dbus.SessionBus ()
        proxy = bus.get_object (dbus.BUS_DAEMON_NAME, dbus.BUS_DAEMON_PATH)
        iface = dbus.Interface (proxy, dbus.BUS_DAEMON_IFACE)
        reply = iface.RequestName (SERVER_DBUS_NAME, 0)

        if reply != 1:
            return False
        return True

    def emit_completed (self, id):
        self.Completed (id, str (id))

    @dbus.service.method(dbus_interface=SERVER_DBUS_IFACE, in_signature='sa{sv}', out_signature='u', sender_keyword='sender')
    def Play (self, event,  properties, sender=None):
        self.id_count += 1
        print 'PLAY (event=%s, id=%d) from %s' % (event, self.id_count, sender)
        for k, v in properties.items ():
            print "+ Property %s = %s" % (k, v)
        self.events[self.id_count] = gobject.timeout_add_seconds (2, self.emit_completed, self.id_count)
        return self.id_count

    @dbus.service.method(dbus_interface=SERVER_DBUS_IFACE, in_signature='u', sender_keyword='sender')
    def Stop (self, id, sender=None):
        print 'STOP (id=%s) from %s' % (id, sender)
        timeout_id = self.events.get (id, 0)
        if timeout_id > 0:
            gobject.source_remove (timeout_id)
            del self.events[id]

    @dbus.service.signal(dbus_interface=SERVER_DBUS_IFACE, signature='us')
    def Completed (self, id, evt):
        print 'EMIT Completed (id=%s)' % id

    @dbus.service.signal(dbus_interface=SERVER_DBUS_IFACE, signature='us')
    def Failed (self, id, evt):
        print 'EMIT Failed (id=%s)' % id

if __name__ == '__main__':
    DBusGMainLoop (set_as_default=True)

    loop = gobject.MainLoop ()
    server = DummyServer ()
    loop.run ()

