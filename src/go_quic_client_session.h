#ifndef GO_QUIC_CLIENT_SESSION_H_
#define GO_QUIC_CLIENT_SESSION_H_

#include "net/quic/quic_client_session_base.h"
#include "net/quic/quic_crypto_client_stream.h"
#include "net/quic/quic_protocol.h"
#include "go_quic_spdy_client_stream.h"

namespace net {

namespace tools {

class GoQuicClientSession : public QuicClientSessionBase {
 public:
  GoQuicClientSession(const QuicConfig& config,
                      QuicConnection* connection,
                      const QuicServerId& server_id,
                      QuicCryptoClientConfig* crypto_config);
  ~GoQuicClientSession() override;
  // Set up the QuicClientSession. Must be called prior to use.
  void Initialize() override;

  // QuicSession methods:
  GoQuicSpdyClientStream* CreateOutgoingDynamicStream(
      SpdyPriority priority) override;
  QuicCryptoClientStreamBase* GetCryptoStream() override;

  // QuicClientSessionBase methods:
  void OnProofValid(const QuicCryptoClientConfig::CachedState& cached) override;
  void OnProofVerifyDetailsAvailable(
      const ProofVerifyDetails& verify_details) override;

  // Performs a crypto handshake with the server
  void CryptoConnect();

  // Returns the number of client hello messages that have been sent on the
  // crypto stream. If the handshake has completed then this is one greater
  // than the number of round-trips needed for the handshake.
  int GetNumSentClientHellos() const;

  void set_respect_goaway(bool respect_goaway) {
    respect_goaway_ = respect_goaway;
  }

 protected:
  // QuicSession methods:
  QuicSpdyStream* CreateIncomingDynamicStream(QuicStreamId id) override;

  // Create the crypto stream. Called by Initialize()
  virtual QuicCryptoClientStreamBase* CreateQuicCryptoStream();

  // Unlike CreateOutgoingDynamicStream, which applies a bunch of sanity checks,
  // this simply returns a new QuicSpdyClientStream. This may be used by
  // subclasses which want to use a subclass of QuicSpdyClientStream for streams
  // but wish to use the sanity checks in CreateOutgoingDynamicStream.
  virtual GoQuicSpdyClientStream* CreateClientStream();

  const QuicServerId& server_id() { return server_id_; }
  QuicCryptoClientConfig* crypto_config() { return crypto_config_; }

 private:
  scoped_ptr<QuicCryptoClientStreamBase> crypto_stream_;
  QuicServerId server_id_;
  QuicCryptoClientConfig* crypto_config_;

  // If this is set to false, the client will ignore server GOAWAYs and allow
  // the creation of streams regardless of the high chance they will fail.
  bool respect_goaway_;

  DISALLOW_COPY_AND_ASSIGN(GoQuicClientSession);
};

}  // namespace tools
}  // namespace net

#endif  // GO_QUIC_CLIENT_SESSION_H_