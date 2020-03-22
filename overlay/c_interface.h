#ifndef f1_overlay_cinterface
#define f1_overlay_cinterface

#ifdef __cplusplus
extern "C"
{
#endif

    void f1_overlay_start();
    void f1_overlay_update_slip(double rl, double rr, double fl, double fr);

#ifdef __cplusplus
}
#endif

#endif  // f1_overlay_cinterface
