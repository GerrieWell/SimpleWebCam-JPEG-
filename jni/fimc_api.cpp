static int fimc_poll(struct pollfd *events)
{
    int ret;

    /* 10 second delay is because sensor can take a long time
     * to do auto focus and capture in dark settings
     */
    /* TODO: Delay changed from 10 secs to 1 sec,
       verify if this has not affected anything else */
    ret = poll(events, 1, 1000);
    if (ret < 0) {
        ALOGE("ERR(%s):poll error", __func__);
        return ret;
    }

    if (ret == 0) {
        ALOGI("WARN(%s):No data in 1 sec..", __func__);
        return ret;
    }

    return ret;
}

static int fimc_v4l2_querycap(int fp)
{
    struct v4l2_capability cap;

    if (ioctl(fp, VIDIOC_QUERYCAP, &cap) < 0) {
        ALOGE("ERR(%s):VIDIOC_QUERYCAP failed", __func__);
        return -1;
    }

    //if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        ALOGE("ERR(%s):no capture devices", __func__);
        return -1;
    }

    return 0;
}

static const __u8* fimc_v4l2_enuminput(int fp, int index)
{
    static struct v4l2_input input;

    input.index = index;
    if (ioctl(fp, VIDIOC_ENUMINPUT, &input) != 0) {
        ALOGE("ERR(%s):No matching index found", __func__);
        return NULL;
    }
    ALOGI("Name of input channel[%d] is %s", input.index, input.name);

    return input.name;
}

static int fimc_v4l2_s_input(int fp, int index)
{
    struct v4l2_input input;

    input.index = index;

    if (ioctl(fp, VIDIOC_S_INPUT, &input) < 0) {
        ALOGE("ERR(%s):VIDIOC_S_INPUT failed", __func__);
        return -1;
    }

    return 0;
}

static int fimc_v4l2_s_fmt(int fp, int width, int height, unsigned int fmt, enum v4l2_field field, unsigned int num_plane)
{
    struct v4l2_format v4l2_fmt;
    struct v4l2_pix_format pixfmt;
    unsigned int framesize;

    memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
    v4l2_fmt.type = V4L2_BUF_TYPE;

    framesize = (width * height * get_pixel_depth(fmt)) / 8;

    v4l2_fmt.fmt.pix_mp.width = width;
    v4l2_fmt.fmt.pix_mp.height = height;
    v4l2_fmt.fmt.pix_mp.pixelformat = fmt;
    v4l2_fmt.fmt.pix_mp.field = field;
    if (num_plane == 1) {
        v4l2_fmt.fmt.pix_mp.plane_fmt[0].sizeimage = framesize;
    } else if (num_plane == 2) {
        v4l2_fmt.fmt.pix_mp.plane_fmt[0].sizeimage = ALIGN(ALIGN(width, 16) * ALIGN(height, 16), 2048);
        v4l2_fmt.fmt.pix_mp.plane_fmt[1].sizeimage = ALIGN(ALIGN(width, 16) * ALIGN(height/2, 8), 2048);
    } else if (num_plane == 3) {
        v4l2_fmt.fmt.pix_mp.plane_fmt[0].sizeimage = ALIGN(width, 16) * ALIGN(height, 16);
        v4l2_fmt.fmt.pix_mp.plane_fmt[1].sizeimage = ALIGN(width/2, 16) * ALIGN(height/2, 16);
        v4l2_fmt.fmt.pix_mp.plane_fmt[2].sizeimage = ALIGN(width/2, 16) * ALIGN(height/2, 16);
    } else {
        ALOGE("ERR(%s): Invalid plane number", __func__);
        return -1;
    }
    v4l2_fmt.fmt.pix_mp.num_planes = num_plane;

    /* Set up for capture */
    if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
        ALOGE("ERR(%s):VIDIOC_S_FMT failed", __func__);
        return -1;
    }

    return 0;
}

static int fimc_v4l2_s_fmt_cap(int fp, int width, int height, unsigned int fmt)
{
    struct v4l2_format v4l2_fmt;
    struct v4l2_pix_format pixfmt;

    memset(&pixfmt, 0, sizeof(pixfmt));

    v4l2_fmt.type = V4L2_BUF_TYPE;

    pixfmt.width = width;
    pixfmt.height = height;
    pixfmt.pixelformat = fmt;
    if (fmt == V4L2_PIX_FMT_JPEG)
        pixfmt.colorspace = V4L2_COLORSPACE_JPEG;

    v4l2_fmt.fmt.pix = pixfmt;
    ALOGI("fimc_v4l2_s_fmt_cap : width(%d) height(%d)", width, height);

    /* Set up for capture */
    if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
        ALOGI("ERR(%s):VIDIOC_S_FMT failed", __func__);
        return -1;
    }

    return 0;
}

int fimc_v4l2_s_fmt_is(int fp, int width, int height, unsigned int fmt, enum v4l2_field field)
{
    /* Return immmediately for now.
    s_fmt_is will be required in future for IS */
    //return 0;
    use_isp=1;
    struct v4l2_format v4l2_fmt;
    struct v4l2_pix_format pixfmt;

    memset(&pixfmt, 0, sizeof(pixfmt));

    v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//V4L2_BUF_TYPE_PRIVATE;

    /*pixfmt.width = width;
    pixfmt.height = height;
    pixfmt.pixelformat = fmt;
    pixfmt.field = field;*/
	v4l2_fmt.fmt.pix.width = width;
	v4l2_fmt.fmt.pix.height = height;
	v4l2_fmt.fmt.pix.pixelformat = fmt;
	v4l2_fmt.fmt.pix.field = field;

    v4l2_fmt.fmt.pix = pixfmt;
    ALOGI("===============fimc_v4l2_s_fmt_is : width(%d) height(%d)", width, height);

    /* Set up for capture */
    if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
        ALOGI("ERR(%s):VIDIOC_S_FMT failed", __func__);
        return -1;
    }

    return 0;
}

static int fimc_v4l2_enum_fmt(int fp, unsigned int fmt)
{
    struct v4l2_fmtdesc fmtdesc;
    int found = 0;
	int ret=1;
	memset(&fmtdesc,0,sizeof(fmtdesc));
    //@wei  fmtdesc.type = V4L2_BUF_TYPE;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtdesc.index = 0;
	// while (ioctl(fp, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
    while (ret>= 0) {
		ret=ioctl(fp, VIDIOC_ENUM_FMT, &fmtdesc);
		ALOGweiV(ret);
		ALOGwei("supported format: %d",fmtdesc.pixelformat);
        if (fmtdesc.pixelformat == fmt) {
            ALOGI("passed fmt = %#x found pixel format[%d]: %s", fmt, fmtdesc.index, fmtdesc.description);
            found = 1;
            break;
        }

        fmtdesc.index++;
    }

    if (!found) {
        ALOGE("unsupported pixel format");
        return -1;
    }

    return 0;
}

static int fimc_v4l2_reqbufs(int fp, enum v4l2_buf_type type, int nr_bufs)
{
    struct v4l2_requestbuffers req;

    req.count = nr_bufs;
    req.type = type;
    req.memory = V4L2_MEMORY_MMAP;
 /*   depend on what ??@wei    V4L2_MEMORY_TYPE;
*/


    if (ioctl(fp, VIDIOC_REQBUFS, &req) < 0) {
        ALOGE("ERR(%s):VIDIOC_REQBUFS failed", __func__);
        return -1;
    }

    return req.count;
}

static int fimc_v4l2_querybuf(int fp, struct SecBuffer *buffers, enum v4l2_buf_type type, int nr_frames, int num_plane)
{
    struct v4l2_buffer v4l2_buf;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
    int i, ret, plane_index;

    for (i = 0; i < nr_frames; i++) {
        v4l2_buf.type = type;
        v4l2_buf.memory = V4L2_MEMORY_MMAP;//V4L2_MEMORY_TYPE;
        v4l2_buf.index = i;
        //v4l2_buf.m.planes = planes;
        //v4l2_buf.length = num_plane;  // this is for multi-planar
        ALOGI("QUERYBUF(index=%d)", i);
        //ALOGI("memory plane is %d", v4l2_buf.length);

        ret = ioctl(fp, VIDIOC_QUERYBUF, &v4l2_buf);
        if (ret < 0) {
            ALOGE("ERR(%s):VIDIOC_QUERYBUF failed", __func__);
            return -1;
        }
#if 1	
		ALOGwei("__LINE_:%d",__LINE__);
        for (plane_index = 0; plane_index < num_plane; plane_index++) {
            //ALOGI("Offset : 0x%x", v4l2_buf.m.planes[plane_index].m.mem_offset);
           // ALOGI("Plane Length : 0x%x", v4l2_buf.m.planes[plane_index].length);

            //buffers[i].size.extS[plane_index] = v4l2_buf.m.planes[plane_index].length;
            //ALOGI("length[%d] : 0x%x", i, buffers[i].size.extS[plane_index]);
            ALOGwei("__LINE_:%d",__LINE__);
            if ((buffers[i].virt.extP[plane_index] = (char *)mmap(0, v4l2_buf.length,
                    PROT_READ, MAP_SHARED, fp, v4l2_buf.m.offset)) < 0) {
                ALOGE("mmap failed");
                return -1;
            }
            //ALOGI("vaddr[%d][%d] : 0x%x", i, plane_index, (__u32) buffers[i].virt.extP[plane_index]);
        }
#endif
		
    }
    return 0;
}

static int fimc_v4l2_streamon(int fp)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret;

    ret = ioctl(fp, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_STREAMON failed", __func__);
        return ret;
    }

    return ret;
}

static int fimc_v4l2_streamoff(int fp)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE;
    int ret;

    ALOGI("%s :", __func__);
    ret = ioctl(fp, VIDIOC_STREAMOFF, &type);
    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_STREAMOFF failed", __func__);
        return ret;
    }

    return ret;
}

static int fimc_v4l2_qbuf(int fp, int width, int height, struct SecBuffer *vaddr, int index, int num_plane, int mode)
{
	struct v4l2_buffer v4l2_buf;
    int ret;
#if 0

    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    v4l2_buf.m.planes = planes;
    v4l2_buf.length = num_plane;

    v4l2_buf.type = V4L2_BUF_TYPE;
    v4l2_buf.memory = V4L2_MEMORY_TYPE;
    v4l2_buf.index = index;
    v4l2_buf.flags = 0;
	
    ret = ioctl(fp, VIDIOC_QBUF, &v4l2_buf);
#endif


	memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));
	v4l2_buf.index = index;
	v4l2_buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fp, VIDIOC_QBUF, &v4l2_buf);

    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_QBUF failed", __func__);
        return ret;
    }

    return 0;
}



static int fimc_v4l2_dqbuf_jpeg(int fp, int num_plane, int *jpeg_size, int *thumb_size)
{
    struct v4l2_buffer v4l2_buf;
    int ret;

    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    v4l2_buf.m.planes = planes;
    v4l2_buf.length = num_plane;

    v4l2_buf.type = V4L2_BUF_TYPE;
    v4l2_buf.memory = V4L2_MEMORY_TYPE;

    ret = ioctl(fp, VIDIOC_DQBUF, &v4l2_buf);
    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_DQBUF failed, dropped frame", __func__);
        return ret;
    }

    *jpeg_size = v4l2_buf.m.planes[0].bytesused;
    *thumb_size = v4l2_buf.m.planes[1].bytesused;

    return v4l2_buf.index;
}

static int fimc_v4l2_dqbuf(int fp, int num_plane)
{
    struct v4l2_buffer v4l2_buf;
    int ret;

    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    //v4l2_buf.m.planes = planes;
    //v4l2_buf.length = num_plane;

    v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fp, VIDIOC_DQBUF, &v4l2_buf);
    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_DQBUF failed, dropped frame", __func__);
        return ret;
    }

    return v4l2_buf.index;
}

static int fimc_v4l2_g_ctrl(int fp, unsigned int id)
{
    struct v4l2_control ctrl;
    int ret;

    ctrl.id = id;

    ret = ioctl(fp, VIDIOC_G_CTRL, &ctrl);
    if (ret < 0) {
        ALOGE("ERR(%s): VIDIOC_G_CTRL(id = 0x%x (%d)) failed, ret = %d",
             __func__, id, id-V4L2_CID_PRIVATE_BASE, ret);
        return ret;
    }

    return ctrl.value;
}

static int fimc_v4l2_s_ctrl(int fp, unsigned int id, unsigned int value)
{
    /* Return immediately for now
    s_ctrl will be required for IS later */

    struct v4l2_control ctrl;
    int ret;
    if(!use_isp)
	return 0;
    ctrl.id = id;
    ctrl.value = value;

    ret = ioctl(fp, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0) {
        ALOGE("ERR(%s):VIDIOC_S_CTRL(id = %#x (%d), value = %d) failed ret = %d",
             __func__, id, id-V4L2_CID_PRIVATE_BASE, value, ret);

        return ret;
    }

    return ctrl.value;
}

static int fimc_v4l2_s_ext_ctrl(int fp, unsigned int id, void *value)
{
    struct v4l2_ext_controls ctrls;
    struct v4l2_ext_control ctrl;
    int ret;

    ctrl.id = id;

    ctrls.ctrl_class = V4L2_CTRL_CLASS_CAMERA;
    ctrls.count = 1;
    ctrls.controls = &ctrl;

    ret = ioctl(fp, VIDIOC_S_EXT_CTRLS, &ctrls);
    if (ret < 0)
        ALOGE("ERR(%s):VIDIOC_S_EXT_CTRLS failed", __func__);

    return ret;
}
