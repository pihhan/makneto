
#ifndef JINGLEERROR_H
#define JINGLEERROR_H

typedef enum {
    NoError = 0,
    AudioInputError,    ///<! Problem with audio source
    AudioOutputError,
    VideoInputError,
    VideoOutputError,
    NetworkError,   ///<! Problem with network, bad addresses or something.
    NetworkTimeoutError, ///<! Remote party did not respond in time.
    PipelineError,  ///<! Pipeline creation failed.
    NegotiationError, ///<! Could not negotiate working parameters.
    IncompatibleCodecError, ///<! Could not negotiate compatible codecs.
    XmppNotImplementedError, ///<! request type not implemented on remote side
    XmppNotAvailableError, ///<! remote target is not reachable.
    UnspecifiedError,
} JingleErrors;

#endif
