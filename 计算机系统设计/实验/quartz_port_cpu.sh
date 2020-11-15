#!/bin/bash
# Ubuntu bash script used to port Quartz on more intel cpu
# 张丘洋 (cs.qyzhang@qq.com) 2020.10.24

# environment
sudo apt update
sudo apt install cmake libconfig-dev libnuma-dev uthash-dev linux-headers-generic msr-tools
if [ ! -d "libpfm-4.11.0" ];
then
    if [ ! -f "libpfm-4.11.0.tar.gz" ];
    then
        wget https://nchc.dl.sourceforge.net/project/perfmon2/libpfm4/libpfm-4.11.0.tar.gz
    fi
    tar xzf libpfm-4.11.0.tar.gz
fi

# Perfmon2
cd libpfm-4.11.0
make
./examples/showevtinfo -E > event.txt

# detect hardware event
for hardware_event in {"CYCLE_ACTIVITY:STALLS_L2_MISS","MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE","MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM"}
do
    if [ -z "$(grep $hardware_event event.txt)" ];
    then
        echo -e "\033[0;31mhardware event $hardware_event not found! you cannot use this script.\033[0m"
        exit 1
    fi
done

cd ..

if [ -d "./quartz" ];
then
    read -p "[31mDetect quartz folder already exist, do you want to delete it?[y/n][0m " -n 1 -r
    if [[ $REPLY =~ ^[Yy]$ ]];
    then
        echo -e "\nremove quartz"
        rm -rf quartz
    else
        echo -e "\nexit."
        exit 0
    fi
fi

git clone https://github.com/HewlettPackard/quartz.git
cd quartz

cat > ./src/lib/cpu/cascadelake-papi.h <<'CATEND'
#ifndef __CPU_CASCADE_LAKE_PAPI_H
#define __CPU_CASCADE_LAKE_PAPI_H

#include <papi.h>
#include "debug.h"

// Perfmon2 is a library that provides a generic interface to access the PMU. It also comes with
// applications to list all available performance events with their architecture specific
// detailed description and translate them to their respective event code. 'showevtinfo' application can
// be used to list all available performance event names with detailed description and 'check_events' application
// can be used to translate the performance event to the corresponding event code.

// These events will be initialized and started.
// Every event reading will return an array with the values for all these events.
// The array index is the same index used to define the event in the *_native_events array below
const char *cascadelake_native_events[MAX_NUM_EVENTS] = {
    "CYCLE_ACTIVITY:STALLS_L2_MISS",
    "MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE",
    "MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM"
    // "MEM_LOAD_UOPS_L3_MISS_RETIRED:REMOTE_DRAM",
};

uint64_t cascadelake_read_stall_events_local() {
    long long values[MAX_NUM_EVENTS];
    uint64_t events = 0;

    if (pmc_events_read_local_thread(values) == PAPI_OK) {
		uint64_t l2_miss = values[0];
		uint64_t llc_hit  = values[1];
		uint64_t local_dram  = values[2];
		// uint64_t remote_dram = values[3];
		uint64_t remote_dram = 0;

		DBG_LOG(DEBUG, "read stall L2 cycles %lu; llc_hit %lu; remote_dram %lu; local_dram %lu\n",
			l2_miss, llc_hit, remote_dram, local_dram);

		double num = remote_dram + local_dram;
		double den = num + llc_hit;
		if (den == 0) return 0;

		events = (uint64_t)((double)l2_miss * ((double)num / den));
    } else {
        DBG_LOG(ERROR, "read stall cycles failed\n");
    }

    return events;
}

uint64_t cascadelake_read_stall_events_remote() {
    long long values[MAX_NUM_EVENTS];
    uint64_t events = 0;

    if (pmc_events_read_local_thread(values) == PAPI_OK) {
		uint64_t l2_miss = values[0];
		uint64_t llc_hit  = values[1];
		uint64_t local_dram  = values[2];
		// uint64_t remote_dram = values[3];
		uint64_t remote_dram = 0;

		DBG_LOG(DEBUG, "read stall L2 cycles %lu; llc_hit %lu; remote_dram %lu; local_dram %lu\n",
			l2_miss, llc_hit, remote_dram, local_dram);

		// calculate stalls based on l2 stalls and LLC miss/hit
		double num = remote_dram + local_dram;
		double den = num + llc_hit;
		if (den == 0) return 0;
		double stalls = (double)l2_miss * ((double)num / den);

		// calculate remote dram stalls based on total stalls and local/remote dram accesses
		den = remote_dram + local_dram;
		if (den == 0) return 0;
		events = (uint64_t) (stalls * ((double)remote_dram / den));
    } else {
        DBG_LOG(ERROR, "read stall cycles failed\n");
    }

    return events;
}

#endif /* __CPU_CASCADE_LAKE_PAPI_H */
CATEND

cat > ./src/lib/cpu/cascadelake.h <<'CATEND'
#ifndef __CPU_CASCADE_LAKE_H
#define __CPU_CASCADE_LAKE_H

#include <math.h>
#include "thread.h"
#include "cpu/pmc.h"
#include "debug.h"

// Perfmon2 is a library that provides a generic interface to access the PMU. It also comes with
// applications to list all available performance events with their architecture specific
// detailed description and translate them to their respective event code. 'showevtinfo' application can
// be used to list all available performance event names with detailed description and 'check_events' application
// can be used to translate the performance event to the corresponding event code.

extern __thread int tls_hw_local_latency;
extern __thread int tls_hw_remote_latency;
#ifdef MEMLAT_SUPPORT
extern __thread uint64_t tls_global_remote_dram;
extern __thread uint64_t tls_global_local_dram;
#endif

#undef FOREACH_PMC_HW_EVENT
#define FOREACH_PMC_HW_EVENT(ACTION)                                                                       \
  ACTION("CYCLE_ACTIVITY:STALLS_L2_MISS", NULL, 0x55305a3)                                              \
  ACTION("MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE", NULL, 0x5308d2)                                        \
  /*ACTION("MEM_LOAD_UOPS_L3_MISS_RETIRED:REMOTE_DRAM", NULL, 0x5302d3)*/                                     \
  ACTION("MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM", NULL, 0x5301d3)

#undef FOREACH_PMC_EVENT
#define FOREACH_PMC_EVENT(ACTION, prefix)                                                                  \
  ACTION(ldm_stall_cycles, prefix)
//   ACTION(remote_dram, prefix)

#define L3_FACTOR 7.0

DECLARE_ENABLE_PMC(cascadelake, ldm_stall_cycles)
{
    ASSIGN_PMC_HW_EVENT_TO_ME("CYCLE_ACTIVITY:STALLS_L2_MISS", 0);
    ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE", 1);
    ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM", 2);
   //  ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_MISS_RETIRED:REMOTE_DRAM", 3);

    return E_SUCCESS;
}

DECLARE_CLEAR_PMC(cascadelake, ldm_stall_cycles)
{
}

DECLARE_READ_PMC(cascadelake, ldm_stall_cycles)
{
   uint64_t l2_pending_diff  = READ_MY_HW_EVENT_DIFF(0);
   uint64_t llc_hit_diff     = READ_MY_HW_EVENT_DIFF(1);
   uint64_t local_dram_diff  = READ_MY_HW_EVENT_DIFF(2);
   // uint64_t remote_dram_diff = READ_MY_HW_EVENT_DIFF(3);
   uint64_t remote_dram_diff = 0;

   DBG_LOG(DEBUG, "read stall L2 cycles diff %lu; llc_hit %lu; cycles diff remote_dram %lu; local_dram %lu\n",
		   l2_pending_diff, llc_hit_diff, remote_dram_diff, local_dram_diff);

   if ((remote_dram_diff == 0) && (local_dram_diff == 0)) return 0;
#ifdef MEMLAT_SUPPORT
   tls_global_local_dram += local_dram_diff;
#endif

   // calculate stalls based on L2 stalls and LLC miss/hit
   double num = L3_FACTOR * (remote_dram_diff + local_dram_diff);
   double den = num + llc_hit_diff;
   if (den == 0) return 0;
   return (uint64_t) ((double)l2_pending_diff * (num / den));
}


DECLARE_ENABLE_PMC(cascadelake, remote_dram)
{
    ASSIGN_PMC_HW_EVENT_TO_ME("CYCLE_ACTIVITY:STALLS_L2_MISS", 0);
    ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE", 1);
    ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM", 2);
   //  ASSIGN_PMC_HW_EVENT_TO_ME("MEM_LOAD_UOPS_L3_MISS_RETIRED:REMOTE_DRAM", 3);

    return E_SUCCESS;
}

DECLARE_CLEAR_PMC(cascadelake, remote_dram)
{
}

DECLARE_READ_PMC(cascadelake, remote_dram)
{
   uint64_t l2_pending_diff  = READ_MY_HW_EVENT_DIFF(0);
   uint64_t llc_hit_diff     = READ_MY_HW_EVENT_DIFF(1);
   uint64_t local_dram_diff  = READ_MY_HW_EVENT_DIFF(2);
   // uint64_t remote_dram_diff = READ_MY_HW_EVENT_DIFF(3);
   uint64_t remote_dram_diff = 0;

   DBG_LOG(DEBUG, "read stall L2 cycles diff %lu; llc_hit %lu; cycles diff remote_dram %lu; local_dram %lu\n",
		   l2_pending_diff, llc_hit_diff, remote_dram_diff, local_dram_diff);

   if ((remote_dram_diff == 0) && (local_dram_diff == 0)) return 0;
#ifdef MEMLAT_SUPPORT
   tls_global_remote_dram += remote_dram_diff;
#endif

   // calculate stalls based on L2 stalls and LLC miss/hit
   double num = L3_FACTOR * (remote_dram_diff + local_dram_diff);
   double den = num + llc_hit_diff;
   if (den == 0) return 0;
   double stalls = (double)l2_pending_diff * (num / den);

   // calculate remote dram stalls based on total stalls and local/remote dram accesses
   // also consider the weight of remote memory access against local memory access
   den = (remote_dram_diff * tls_hw_remote_latency) + (local_dram_diff * tls_hw_local_latency);
   if (den == 0) return 0;
   return (uint64_t) (stalls * ((double)(remote_dram_diff * tls_hw_remote_latency) / den));
}


PMC_EVENTS(cascadelake, 3)
#endif /* __CPU_CASCADE_LAKE_H */
CATEND

sed -i "s/0x55305a3/$(grep CYCLE_ACTIVITY:STALLS_L2_MISS ../libpfm-4.11.0/event.txt | awk '{print $1}')/g" ./src/lib/cpu/cascadelake.h
sed -i "s/0x5308d2/$(grep MEM_LOAD_UOPS_L3_HIT_RETIRED:XSNP_NONE ../libpfm-4.11.0/event.txt | awk '{print $1}')/g" ./src/lib/cpu/cascadelake.h
sed -i "s/0x5301d3/$(grep MEM_LOAD_UOPS_L3_MISS_RETIRED:LOCAL_DRAM ../libpfm-4.11.0/event.txt | awk '{print $1}')/g" ./src/lib/cpu/cascadelake.h

patch -p1 <<'PATCHEND'
diff --git a/src/lib/cpu/cpu.c b/src/lib/cpu/cpu.c
index 11bb409..cb52bcf 100644
--- a/src/lib/cpu/cpu.c
+++ b/src/lib/cpu/cpu.c
@@ -212,6 +212,9 @@ cpu_model_t *cpu_model()
             case Haswell:
                 cpu_model = &cpu_model_intel_xeon_ex_v3;
                 break;
+            case CascadeLake:
+                cpu_model = &cpu_model_intel_xeon_ex_cascadelake;
+                break;
             default:
                 return NULL;
             }
diff --git a/src/lib/cpu/cpu.h b/src/lib/cpu/cpu.h
index 6f92545..1b4a7d0 100644
--- a/src/lib/cpu/cpu.h
+++ b/src/lib/cpu/cpu.h
@@ -39,7 +39,9 @@ typedef enum {
     IvyBridge,
     IvyBridgeXeon,
     Haswell,
-    HaswellXeon
+    HaswellXeon,
+    CascadeLake,
+    CascadeLakeXeon
 } microarch_t;
 
 typedef struct
diff --git a/src/lib/cpu/known_cpus.h b/src/lib/cpu/known_cpus.h
index 384e7d6..0ece73a 100644
--- a/src/lib/cpu/known_cpus.h
+++ b/src/lib/cpu/known_cpus.h
@@ -37,6 +37,8 @@ microarch_ID_t known_cpus[] =
         {.family = 0x06, .model = 0x45, .microarch = Haswell},
         {.family = 0x06, .model = 0x46, .microarch = Haswell},
 
+        {.family = 0x66, .model = 0x88, .microarch = CascadeLake},
+
         // must be the last element
         {.family = 0x0, .model = 0x0, .microarch = Invalid}};
 
@@ -49,6 +52,8 @@ char *microarch_strings[] =
         "Ivy Bridge",
         "Ivy Bridge Xeon",
         "Haswell",
-        "Haswell Xeon"};
+        "Haswell Xeon",
+        "Cascade Lake",
+        "Cascade Xeon"};
 
 #endif /* __KNOWN_CPUS_H */
diff --git a/src/lib/cpu/xeon-ex.h b/src/lib/cpu/xeon-ex.h
index 7c87226..2956081 100644
--- a/src/lib/cpu/xeon-ex.h
+++ b/src/lib/cpu/xeon-ex.h
@@ -17,10 +17,12 @@ Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 #include "sandybridge-papi.h"
 #include "ivybridge-papi.h"
 #include "haswell-papi.h"
+#include "cascadelake-papi.h"
 #else
 #include "sandybridge.h"
 #include "ivybridge.h"
 #include "haswell.h"
+#include "cascadelake.h"
 #endif
 
 int intel_xeon_ex_set_throttle_register(pci_regs_t *regs, throttle_type_t throttle_type, uint16_t val)
@@ -110,3 +112,14 @@ cpu_model_t cpu_model_intel_xeon_ex_v3 = {
     .set_throttle_register = intel_xeon_ex_set_throttle_register,
     .get_throttle_register = intel_xeon_ex_get_throttle_register
 };
+
+cpu_model_t cpu_model_intel_xeon_ex_cascadelake = {
+    .microarch = CascadeLakeXeon,
+#ifdef PAPI_SUPPORT
+    .pmc_events = {cascadelake_native_events, cascadelake_read_stall_events_local, cascadelake_read_stall_events_remote},
+#else
+    .pmc_events = PMC_EVENTS_PTR(cascadelake),
+#endif
+    .set_throttle_register = intel_xeon_ex_set_throttle_register,
+    .get_throttle_register = intel_xeon_ex_get_throttle_register
+};
\ No newline at end of file
diff --git a/scripts/turboboost.sh b/scripts/turboboost.sh
index 398fbad..65284e3 100755
--- a/scripts/turboboost.sh
+++ b/scripts/turboboost.sh
@@ -34,14 +34,7 @@ function verify_cpu_id()
 function check_msr_module()
 {
     lsmod | grep msr > /dev/null
-    if [ $? -ne 0 ]; then
-         # some systems need this, others don't
-        sudo modprobe msr &> /dev/null
-        #if [ $? -ne 0 ]; then
-        #    echo "Failed to load MSR module"
-        #    exit 1
-        #fi
-    fi
+    sudo modprobe msr
 }
 
 function check()
@@ -76,9 +76,10 @@ function enable()
     cpus=$(lscpu | sed -n 4p | awk '{ print $2 }')
 
     if [ -z "${cpu}" ]; then
-        for (( i=0; i<${cpus}; i++ )); do 
-            sudo wrmsr -p$i 0x1a0 0x850089
-        done
+        # for (( i=0; i<${cpus}; i++ )); do
+        #     sudo wrmsr -p$i 0x1a0 0x850089
+        # done
+        sudo wrmsr -a 0x1a0 0x850089
         echo "Turbo Boost enabled for all CPUs"
     else
         if [ ${cpu} -ge ${cpus} ]; then
@@ -97,9 +98,10 @@ function disable()
     cpus=$(lscpu | sed -n 4p | awk '{ print $2 }')
 
     if [ -z "${cpu}" ]; then
-        for (( i=0; i<${cpus}; i++ )); do 
-            sudo wrmsr -p$i 0x1a0 0x4000850089;
-        done
+        # for (( i=0; i<${cpus}; i++ )); do
+        #     sudo wrmsr -p$i 0x1a0 0x4000850089;
+        # done
+        sudo wrmsr -a 0x1a0 0x4000850089;
         echo "Turbo Boost disabled for all CPUs"
     else
         if [ ${cpu} -ge ${cpus} ]; then

PATCHEND

sed -i "s/0x88/$(grep -E "model\s+:" /proc/cpuinfo | uniq | awk '{print $3}')/g" ./src/lib/cpu/known_cpus.h
sed -i "s/0x66/$(grep "cpu family" /proc/cpuinfo | uniq | awk '{print $4}')/g" ./src/lib/cpu/known_cpus.h

mkdir build && cd build
cmake ..
make all

echo -e "\033[0;31msuccess!\033[0m"

read -p "[31mDo you want to delete Perfmon2?[y/n][0m " -n 1 -r
if [[ $REPLY =~ ^[Yy]$ ]];
then
    rm -rf ../../libpfm-4.11.0
fi
echo

