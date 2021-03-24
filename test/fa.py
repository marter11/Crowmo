dat = "\x80\x60\x00\x01\x00\x00\x35\x20\x6b\x8b\x45\x67"\
"\x47\x4d\x00\x1f\x99\xa0\x14\x01\x6e\x84\x00\x00\x0f\xa0\x00\x02\x71\x02\x10"

print(dat[0])

# with open("fal.png", "wb") as f:
#     f.write(o.encode())


# avcodec_init();  
# avcodec_register_all();
# AVCodec* pCodec;
# pCodec=lpavcodec_find_decoder(CODEC_ID_H264);
# AVCodecContext* pCodecContext;
# pCodecContext=lpavcodec_alloc_context();
# avcodec_open(pCodecContext,pCodec);
# AVFrame *pFrame;
# pFrame=avcodec_alloc_frame();
# //for every nal unit:
#     int frameFinished=0;
#     //nalData2 is nalData without the first 4 bytes
#     avcodec_decode_video(pCodecContext,pFrame,&frameFinished,(uint8_t*) nalData2,nalLength);
