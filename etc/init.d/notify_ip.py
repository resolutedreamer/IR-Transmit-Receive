import smtplib
import subprocess
import sys
import time
import threading
import datetime
from email.mime.text import MIMEText

SMTP_SERVER = "smtp.gmail.com:587"
SMTP_UNAME = "enerdash@gmail.com"
SMTP_PASSWD = "enerdash!"
DEFAULT_NOTIFY_PAUSE = 3600
DEFAULT_CHECK_INTERVAL = 60
CELL_SERVICE_DOMAIN = {
        "att": "txt.att.net",
        "verizon": "vtext.com",
        "tmobile": "tmomail.net",
        "sprint": "messaging.sprintpcs.com",
        "virgin": "vmobl.com",
        "uscellular": "email.uscc.net",
        "nextel": "messaging.nextel.com",
        "boost": "myboostmobile.com",
        "alltel": "message.alltel.com"
}

class Notification(object):
    """The base class for all notification objects.

    Notification subclasses must implement the notify() method, which
    actually sends the notification.
    """

    def __init__(self, condition, arg, notify_pause=DEFAULT_NOTIFY_PAUSE):
        """
        Args:
        condition (function): A function that takes one argument (arg) and
            and returns a boolean, which indicates whether the notification
            should be sent.
        arg (any) : arg is passed to condition function. It can be anything
            the user wants.
        notify_pause (int, optional): The number of seconds to wait after
            sending a notification before sending a repeat notification.
        """
        self.condition = condition
        self.arg = arg
        self.notify_pause = notify_pause

    def try_notify(self):
        """Tries to send the notification if the condition is satisfied and
        we haven't already sent a notification too recently.
        """
        if self.last_notify_time == 0:
            notify_ready_time = 0
        else:
            notify_ready_time = self.last_notify_time + self.notify_pause

        if self.condition(self.arg) and notify_ready_time < time.time():
            self.notify()
            self.last_notify_time = time.time()

class EmailNotification(Notification):
    """Sends email notifications"""

    def __init__(self, email, msg, condition, arg,
            notify_pause=DEFAULT_NOTIFY_PAUSE):
        """
        Args:
        email (string): The email to send the notification to.
        msg (string): The message to send in the email.
        condition, arg, notify_pause: Same as for Notification.
        """
        self.email = email
        self.msg = msg
        super(EmailNotification, self).__init__(condition, arg,
                notify_pause)
        self.last_notify_time = 0

    def notify(self):
        """Sends the email notification"""
        subject = "Energy Dashboard Notification"
        from_addr = "enerdash@gmail.com"
        if hasattr(self.msg, "__call__"):
            mimetext = MIMEText(self.msg())
        else:
            mimetext = MIMEText(self.msg)
        mimetext["Subject"] = subject
        mimetext["From"] = from_addr
        mimetext["To"] = self.email

        server = smtplib.SMTP(SMTP_SERVER)
        server.starttls()
        server.login(SMTP_UNAME, SMTP_PASSWD)
        server.sendmail(from_addr, [self.email], mimetext.as_string())
        server.quit()

class TxtNotification(EmailNotification):
    """Sends text message notifications"""

    def __init__(self, number, service, msg, condition, args,
            notify_pause=DEFAULT_NOTIFY_PAUSE):
        """
        Args:
        number (int or string): The phone number to receive the text
            message.
        service (string): Must be one of the keys of CELL_SERVICE_DOMAIN.
        msg (string): The content of the text message.
        condition, args, notify_pause: Same as for Notification.
        """
        email = str(number) + "@" + CELL_SERVICE_DOMAIN[service]
        super(TxtNotification, self).__init__(email, msg, condition, args,
                notify_pause)

class NotificationManager(threading.Thread):
    """Thread that will continue to try to send notifications if the
    notification conditions are satisfied.
    """
    def __init__(self, check_interval=DEFAULT_CHECK_INTERVAL):
        """
        Args:
        check_interval (int, optional): The number of seconds to wait in
            between checking the conditions of the notifications.
        """
        super(NotificationManager, self).__init__()
        self.notifications = []
        self.active = False
        self.check_interval = check_interval

    def add_notification(self, notification):
        """Adds a notification to monitor.

        Args:
        notification (subclass of Notification): The notification to
            monitor.
        """
        self.notifications.append(notification)

    def start_notifications(self):
        """Start the notification thread."""
        self.active = True
        self.start()

    def stop_notifications(self):
        """Stop the notification thread."""
        self.active = False

    def run(self):
        """Runs inside the thread. You don't need to call this."""
        while self.active:
            self._send_notifications()
            time.sleep(self.check_interval)

    def _send_notifications(self):
        for notif in self.notifications:
            notif.try_notify()

if __name__ == "__main__":
    time.sleep(10)
    nm = NotificationManager()
    output = subprocess.check_output("ifconfig", shell=True)
    addr_index = output.rfind('inet addr:')
    ip_start_index = (addr_index + 10)
    ip_max_index = (ip_start_index + 16)
    ip = output[ip_start_index:ip_max_index]
    ip_true_end_index = ip.find(' ')
    ip = ip[:ip_true_end_index]
    msg = "Time is: " + str(datetime.datetime.now()) + '\n' + 'ip is ' + ip
    email_notification = EmailNotification("resolutedreamer@live.com", msg, lambda x: True, None, notify_pause=60)
    txtnot = TxtNotification("9512852593", "att", msg,
           lambda x: True, None, notify_pause=60)
    txtnot1 = TxtNotification("3108000493", "tmobile", msg,
           lambda x: True, None, notify_pause=60)
    nm.add_notification(email_notification)
    nm.add_notification(txtnot)
    nm.add_notification(txtnot1)
    
    nm.start_notifications()
    time.sleep(5)
    nm.stop_notifications()
    sys.exit()
