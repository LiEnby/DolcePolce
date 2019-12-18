
#include <psp2/kernel/modulemgr.h>
#include <taihen.h>
#include <string.h>
#include <vitasdk.h>

static SceUID check_psm_allowed_hook = -1;
static tai_hook_ref_t check_psm_allowed_hook_ref;

static SceUID check_vita_allowed_hook = -1;
static tai_hook_ref_t check_vita_allowed_hook_ref;

static SceUID check_pspemu_allowed_hook = -1;
static tai_hook_ref_t check_pspemu_allowed_hook_ref;


static int check_psm_allowed(char **game, int *is_allowed, int *unk) {
    int ret = TAI_CONTINUE(int, check_psm_allowed_hook_ref,game, is_allowed, unk);
	sceClibPrintf("[DolcePolce] check_psm_allowed(%s,%x,%x); ret = %x!\n",*game, *is_allowed, *unk, ret);
	
	if(*is_allowed <= 0)
	{
		sceClibPrintf("[DolcePolce] check_psm_allowed: %s is blacklisted... launching anyway!\n",*game);
		*is_allowed = 1;
	}
	
	return ret;
}

static int check_vita_allowed(char **game, int *is_allowed, int *unk) {
    int ret = TAI_CONTINUE(int, check_vita_allowed_hook_ref, game, is_allowed, unk);
	sceClibPrintf("[DolcePolce] check_vita_allowed(%s,%x,%x); ret = %x!\n",*game, *is_allowed, *unk, ret);
	
	if(*is_allowed <= 0)
	{
		sceClibPrintf("[DolcePolce] check_vita_allowed: %s is blacklisted... launching anyway!\n",*game);
		*is_allowed = 1;
	}
	
	return ret;
}

static int check_pspemu_allowed(char **game, int *is_allowed, int *unk) {
    int ret = TAI_CONTINUE(int, check_pspemu_allowed_hook_ref, game, is_allowed, unk);
	sceClibPrintf("[DolcePolce] check_pspemu_allowed(%s,%x,%x); ret = %x!\n",*game, *is_allowed, *unk, ret);
	
	if(*is_allowed <= 0)
	{
		sceClibPrintf("[DolcePolce] check_pspemu_allowed %s is blacklisted... launching anyway!\n",*game);
		*is_allowed = 1;
	}
	
	return ret;
}




void _start() __attribute__ ((weak, alias ("module_start"))); 
int module_start(SceSize argc, const void *args) {

	sceClibPrintf("[DolcePolce] Loaded.. Blessed Be..\n");

	tai_module_info_t tai_info;
	tai_info.size = sizeof(tai_module_info_t);
	taiGetModuleInfo("SceShell", &tai_info);
		
	// this is getting absolutely rediculous tbh but: 
	switch (tai_info.module_nid) { 
		case 0x0552F692: // retail 3.60 SceShell
			check_psm_allowed_hook = taiHookFunctionOffset(&check_psm_allowed_hook_ref, tai_info.modid,0,0x2E8D0,1, check_psm_allowed);
			check_vita_allowed_hook = taiHookFunctionOffset(&check_vita_allowed_hook_ref, tai_info.modid,0,0x2E428,1, check_vita_allowed);
			check_pspemu_allowed_hook = taiHookFunctionOffset(&check_pspemu_allowed_hook_ref, tai_info.modid,0,0x2E734,1, check_pspemu_allowed);
			break;
		case 0x5549BF1F: // retail 3.65 SceShell
		case 0x34B4D82E: // retail 3.67 SceShell
		case 0x12DAC0F3: // retail 3.68 SceShell
		case 0x0703C828: // retail 3.69 SceShell
		case 0x2053B5A5: // retail 3.70 SceShell
		case 0xF476E785: // retail 3.71 SceShell
		case 0x939FFBE9: // retail 3.72 SceShell
		case 0x734D476A: // retail 3.73 SceShell
		default:
			check_psm_allowed_hook = taiHookFunctionOffset(&check_psm_allowed_hook_ref, tai_info.modid,0,0x2E928,1, check_psm_allowed);
			check_vita_allowed_hook = taiHookFunctionOffset(&check_vita_allowed_hook_ref, tai_info.modid,0,0x2E480,1, check_vita_allowed);
			check_pspemu_allowed_hook = taiHookFunctionOffset(&check_pspemu_allowed_hook_ref, tai_info.modid,0,0x2E78C,1, check_pspemu_allowed);
			break;
	}
	
	return SCE_KERNEL_START_SUCCESS;

}

int module_stop(SceSize argc, const void *args) {

  // release hooks
	if (check_psm_allowed_hook >= 0) taiHookRelease(check_psm_allowed_hook, check_psm_allowed_hook_ref);
	if (check_vita_allowed_hook >= 0) taiHookRelease(check_vita_allowed_hook, check_vita_allowed_hook_ref);
	if (check_pspemu_allowed_hook >= 0) taiHookRelease(check_pspemu_allowed_hook, check_pspemu_allowed_hook_ref);

	
  return SCE_KERNEL_STOP_SUCCESS;
}
