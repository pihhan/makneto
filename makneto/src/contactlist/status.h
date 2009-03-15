#ifndef STATUS_H
#define STATUS_H

#include <QString>

class ContactListStatus
{
public:
    enum Type { Online, Away, FFC, XA, DND, Offline, Invisible };

    ContactListStatus(Type type, const QString& message = QString(), int priority = 0) 
        : type_(type), message_(message), priority_(priority) {}

    Type type() const { return type_; }
    const QString& message() const { return message_; }
    int priority() const { return priority_; }

    /*! \brief Create operator for comparing importance of different statuses.
     * \author Petr Mensik <pihhan@cipis.net> */
    bool operator<(const ContactListStatus &other) const 
    {
        return (priority_ < other.priority_);
    }

private:
    Type type_;
    QString message_;
    int  priority_;
};

#endif
