#!/usr/bin/python

import os, sys
import gobject
import dbus, dbus.service, dbus.lowlevel
from dbus.mainloop.glib import DBusGMainLoop

SERVER_DBUS_NAME = 'com.nokia.NonGraphicFeedback1'
SERVER_DBUS_PATH = '/com/nokia/NonGraphicFeedback1'
SERVER_DBUS_IFACE = 'com.nokia.NonGraphicFeedback1'

class Event(object):
    def __init__ (self, sender, event_id):
        self.sender = sender
        self.event_id = event_id

class DummyServer (dbus.service.Object):
    def __init__ (self, *kw, **kwargs):
        self.id_count = 0
        self.events = {}
        self.bus = dbus.SessionBus ()

        dbus.service.Object.__init__ (self, self.bus, SERVER_DBUS_PATH)
        self._request_name ()

    def _request_name (self):
        proxy = self.bus.get_object (dbus.BUS_DAEMON_NAME, dbus.BUS_DAEMON_PATH)
        iface = dbus.Interface (proxy, dbus.BUS_DAEMON_IFACE)
        reply = iface.RequestName (SERVER_DBUS_NAME, 0)

        if reply != 1:
            return False
        return True

    def emit_completed (self, evt):
        self.Completed (evt.sender, evt.event_id)

    @dbus.service.method(dbus_interface=SERVER_DBUS_IFACE, in_signature='sa{sv}', out_signature='u', sender_keyword='sender')
    def Play (self, event, properties, sender=None):
        self.id_count += 1
        print 'PLAY (event=%s, id=%d) from %s' % (event, self.id_count, sender)
        for k, v in properties.items ():
            print "+ Property %s = %s" % (k, v)

        self.events[self.id_count] = gobject.timeout_add_seconds (2, self.emit_completed, Event(sender, self.id_count))
        return self.id_count

    @dbus.service.method(dbus_interface=SERVER_DBUS_IFACE, in_signature='u', sender_keyword='sender')
    def Stop (self, id, sender=None):
        print 'STOP (id=%s) from %s' % (id, sender)
        timeout_id = self.events.get (id, 0)
        if timeout_id > 0:
            gobject.source_remove (timeout_id)
            del self.events[id]

    def Completed (self, destination, id):
        msg = dbus.lowlevel.MethodCallMessage (destination=destination, path=SERVER_DBUS_PATH, interface=SERVER_DBUS_IFACE, method='Status')
        msg.append (id, 0, signature='uu')
        msg.set_no_reply (True)

        self.bus.send_message (msg)

        print 'EMIT (id=%s) to %s' % (id, destination)

if __name__ == '__main__':
    DBusGMainLoop (set_as_default=True)

    loop = gobject.MainLoop ()
    server = DummyServer ()
    loop.run ()

