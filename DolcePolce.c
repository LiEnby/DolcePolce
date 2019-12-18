
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

	/*
	*	Read Fw Version
	*/
	
	SceKernelFwInfo fwInfo;
	fwInfo.size = sizeof(SceKernelFwInfo);
	_vshSblGetSystemSwVersion(&fwInfo);
	int fw = fwInfo.version;
	
	sceClibPrintf("[DolcePolce] Loaded! running on FW: %x\n",fw);

	/*
	*	Get SceShell Info
	*/
	tai_module_info_t tai_info;
	tai_info.size = sizeof(tai_module_info_t);
	taiGetModuleInfo("SceShell", &tai_info);
		
	if(fw >= 0x3650000)
	{
		check_psm_allowed_hook = taiHookFunctionOffset(&check_psm_allowed_hook_ref, tai_info.modid,0,0x2E928,1, check_psm_allowed);
		check_vita_allowed_hook = taiHookFunctionOffset(&check_vita_allowed_hook_ref, tai_info.modid,0,0x2E480,1, check_vita_allowed);
		check_pspemu_allowed_hook = taiHookFunctionOffset(&check_pspemu_allowed_hook_ref, tai_info.modid,0,0x2E78C,1, check_pspemu_allowed);
	}
	else
	{
		check_psm_allowed_hook = taiHookFunctionOffset(&check_psm_allowed_hook_ref, tai_info.modid,0,0x2E8D0,1, check_psm_allowed);
		check_vita_allowed_hook = taiHookFunctionOffset(&check_vita_allowed_hook_ref, tai_info.modid,0,0x2E428,1, check_vita_allowed);
		check_pspemu_allowed_hook = taiHookFunctionOffset(&check_pspemu_allowed_hook_ref, tai_info.modid,0,0x2E734,1, check_pspemu_allowed);
	}
	
	
	sceClibPrintf("[DolcePolce] check_psm_allowed: %x %x\n",check_psm_allowed_hook,check_psm_allowed_hook_ref);
	sceClibPrintf("[DolcePolce] check_vita_allowed: %x %x\n",check_vita_allowed_hook,check_vita_allowed_hook_ref);
	sceClibPrintf("[DolcePolce] check_pspemu_allowed: %x %x\n",check_pspemu_allowed_hook,check_pspemu_allowed_hook_ref);
	
	return SCE_KERNEL_START_SUCCESS;

}

int module_stop(SceSize argc, const void *args) {

  // release hooks
	if (check_psm_allowed_hook >= 0) taiHookRelease(check_psm_allowed_hook, check_psm_allowed_hook_ref);
	if (check_vita_allowed_hook >= 0) taiHookRelease(check_vita_allowed_hook, check_vita_allowed_hook_ref);
	if (check_pspemu_allowed_hook >= 0) taiHookRelease(check_pspemu_allowed_hook, check_pspemu_allowed_hook_ref);

	
  return SCE_KERNEL_STOP_SUCCESS;
}
