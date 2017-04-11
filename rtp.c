#include <ortp/ortp.h>
#include <stdlib.h>


//static const char *help="usage: rtpsend	filename dest_ip4addr dest_port [ --with-clockslide <value> ] [ --with-jitter <milliseconds>]\n";

static RtpSession rtp_session;
static RtpSession *session = &rtp_session;
uint32_t user_ts = 0;
uint32_t frame_counter = 0;

int rtp_init(char *host, int port)
{
	char *ssrc;
	
	ortp_init();
	ortp_scheduler_init();
	ortp_set_log_level_mask(ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR);
	session=rtp_session_new(RTP_SESSION_SENDONLY);	
	
	rtp_session_set_scheduling_mode(session,1);
	rtp_session_set_blocking_mode(session,1);
	rtp_session_set_connected_mode(session,TRUE);
	rtp_session_set_remote_addr(session, host , port);
	rtp_session_set_payload_type(session, 96);

	/* Create format */
	
	//payload_type_set_send_fmtp(PayloadType *pt, const char *fmtp);
	
        /* Create session profile */
	RtpProfile *profile = rtp_profile_new("cam");
	rtp_profile_set_payload(profile, 96, &payload_type_h264);
	rtp_session_set_profile(session, profile);
	
	ssrc=getenv("SSRC");
	if (ssrc!=NULL) {
		printf("using SSRC=%i.\n",atoi(ssrc));
		rtp_session_set_ssrc(session,atoi(ssrc));
	}
	return 0;
}	

int rtp_send(unsigned char *buffer, int len)
{
        int ret;
        //mblk_t *m;
        //m = rtp_session_create_packet(session,
        //        RTP_FIXED_HEADER_SIZE,(uint8_t*)buffer,len);
        
        if (frame_counter++ > 30) {
	    user_ts+=1;
            //rtp_set_markbit(m,1);
	    //fprintf(stderr, "fps = %d, SET MARKBIT %d", frame_counter, rtp_get_markbit(m));
        }

	//fprintf(stderr, "MARKBIT = %d\n", rtp_get_markbit(m));
	//ret=rtp_session_sendm_with_ts(session,m,user_ts);
	ret = rtp_session_send_with_ts(session,buffer,len,user_ts);
        return ret;
}


void rtp_exit() {
	rtp_session_destroy(session);
	ortp_exit();
	ortp_global_stats_display();
}
