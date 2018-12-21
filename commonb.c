/*----------------------------------------------------------------------
| Copyright 1995-2017 Mersenne Research, Inc.  All rights reserved
|
| This file contains routines and global variables that are common for
| all operating systems the program has been ported to.  It is included
| in one of the source code files of each port.
|
| Commona contains information used only during setup
| Commonb contains information used only during execution
| Commonc contains information used during setup and execution
+---------------------------------------------------------------------*/

/* Routines missing from GMP */

#define mpz_add_si(d,s,addin)	if (addin >= 0) mpz_add_ui(d,s,(unsigned int)addin); else mpz_sub_ui(d,s,(unsigned int)-addin);
#define mpz_mul_d(d,s,flt)	{ mpz_t t; mpz_init_set_d(t,flt); mpz_mul(d,s,t); mpz_clear(t); }
#define mpz_eq(a,b)		mpz_cmp(a,b) == 0

/* Globals for error messages */

static const char ERRMSG0[] = "Iteration: %ld/%ld, %s";
static const char ERRMSG1A[] = "ERROR: ILLEGAL SUMOUT\n";
static const char ERRMSG1B[] = "ERROR: SUM(INPUTS) != SUM(OUTPUTS), %.16g != %.16g\n";
static const char ERRMSG1C[] = "Possible error: round off (%.10g) > %.5g\n";
static const char ERRMSG1D[] = "ERROR: Shift counter corrupt.\n";
static const char ERRMSG1E[] = "ERROR: Illegal double encountered.\n";
static const char ERRMSG1F[] = "ERROR: FFT data has been zeroed!\n";
static const char ERRMSG1G[] = "ERROR: Jacobi error check failed!\n";
static const char ERRMSG2[] = "Possible hardware failure, consult readme.txt file.\n";
static const char ERRMSG3[] = "Continuing from last save file.\n";
static const char ERRMSG4[] = "Waiting five minutes before restarting.\n";
static const char ERRMSG5[] = "For added safety, redoing iteration using a slower, more reliable method.\n";
static const char ERRMSG6[] = "ERROR: Comparing PRP double-check values failed.  Rolling back to iteration %lu.\n";
static const char ERRMSG7[] = "ERROR: Comparing Gerbicz checksum values failed.  Rolling back to iteration %lu.\n";
static const char ERROK[] = "Disregard last error.  Result is reproducible and thus not a hardware problem.\n";
static const char READFILEERR[] = "Error reading intermediate file: %s\n";
static const char WRITEFILEERR[] = "Error writing intermediate file: %s\n";
static const char ALTSAVE_MSG[] = "Trying backup intermediate file: %s\n";
static const char ALLSAVEBAD_MSG[] = "All intermediate files bad.  Temporarily abandoning work unit.\n";

/* PauseWhileRunning globals */

struct pause_info {
	int	thread_num;		/* Worker thread to pause */
	int	low_mem;		/* Flag set for LowMemWhileRunning entries */
	int	workers_affected;	/* Number of workers affected */
	char	*program_name;		/* Pause if running this program */
	char	matching_program[80];	/* Running program that matched this entry */
	struct pause_info *next;	/* Next in linked list of program names */
};

int	PAUSE_MUTEX_INITIALIZED = 0;
gwmutex	PAUSE_MUTEX;		/* Lock for accessing pause globals */
struct pause_info *PAUSE_DATA = NULL;
int	PAUSE_WHILE_RUNNING_FREQ = 10;

/* Globals for stopping and starting worker threads */

/* Note that we have one flag byte for each worker thread.  We could */
/* use one bit per worker thread, but then we need to have locks around */
/* updates so that 2 worker threads don't interleave a read-modify-write */
/* operation. */

int	STOP_FOR_RESTART = FALSE;/* Flag indicating we should stop and */
				/* restart all worker threads because an */
				/* important option changed in the GUI. */
				/* One example is changing the priority */
				/* for worker threads. */
int	STOP_FOR_REREAD_INI = FALSE;/* Flag indicating all workers must */
				/* stop because a during/else time period */
				/* has ended and INI file must be reread. */
char	STOP_FOR_MEM_CHANGED[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating it is time to stop */
				/* workers due to day/night memory change. */
int	STOP_FOR_BATTERY = FALSE;/* Flag indicating it is time to stop */
				/* workers due to running on battery. */
int	STOP_FOR_AUTOBENCH = FALSE;/* Flag indicating we chould temporarily */
				/* stop workers to run an auto-benchmark. */
char	STOP_FOR_PRIORITY_WORK[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating it is time to switch */
				/* a worker to high priority work. */
struct pause_info *STOP_FOR_PAUSE[MAX_NUM_WORKER_THREADS] = {NULL};
				/* Flags saying worker thread should */
				/* pause while another program runs */
struct pause_info *STOP_FOR_LOW_MEMORY = NULL; /* Set when LowMemWhileRunning active */
				/* Workers using lots of memory will be stopped */
int	STOP_FOR_LOADAVG = 0;	/* Count of workers to pause due */
				/* to a period of high system load. */
char	STOP_FOR_THROTTLE[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating it is time to pause */
				/* a worker for throttling. */
char	STOP_FOR_ABORT[MAX_NUM_WORKER_THREADS] = {0};
				/* Abort work unit due to unreserve, factor */
				/* found in a different thread, server */
				/* request, or any other reason. */
char	ACTIVE_WORKERS[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating which worker threads */
				/* are active. */
char	WRITE_SAVE_FILES[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating it is time to write */
				/* a save file. */
char	JACOBI_ERROR_CHECK[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating it is time to execute */
				/* a Jacobi error check. */

char	WORK_AVAILABLE_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent WORK_AVAILABLE_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling primeContinue that */
				/* work is now available or all threads */
				/* are stopping */
char	USER_START_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent	USER_START_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling implement_stop_one_worker */
				/* that the user wants this worker to start or */
				/* all threads are stopping */
char	END_PAUSE_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent END_PAUSE_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling implement_pause */
				/* that the pause has ended or */
				/* all threads are stopping */
char	END_LOADAVG_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent END_LOADAVG_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling implement_loadavg */
				/* that the load average condition has ended */
				/* or all threads are stopping */
char	OFF_BATTERY_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent OFF_BATTERY_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling implement_stop_battery */
				/* that AC power has been restored or */
				/* all threads are stopping */
char	MEM_WAIT_OR_STOP_INITIALIZED[MAX_NUM_WORKER_THREADS] = {0};
gwevent MEM_WAIT_OR_STOP[MAX_NUM_WORKER_THREADS] = {0};
				/* Signal for telling avail_mem that */
				/* it can now determine the available memory */

/* Globals for memory manager */

#define DEFAULT_MEM_USAGE 24	/* 24MB default */
unsigned long AVAIL_MEM = 0;	/* Memory available now */
unsigned long MAX_MEM = 0;	/* Max memory available */
unsigned long AVAIL_MEM_PER_WORKER[MAX_NUM_WORKER_THREADS] = {0};
				/* Maximum memory each worker can use */
unsigned long MAX_HIGH_MEM_WORKERS =  0; /* Maximum number of workers */
				/* allowed to use lots of memory */
#define MEM_USAGE_NOT_SET 0x1	/* The mem_in_use value is just a guess */
				/* as the work unit for the thread has not */
				/* started yet or is restarting. */
#define MEM_RESTARTING	0x2	/* The mem_in_use value will be changing */
				/* soon as the thread is being restarted */
				/* because it was using too much memory. */
#define MEM_WILL_BE_VARIABLE_USAGE 0x4
				/* The current work unit will be a */
				/* variable memory user.  We just don't */
				/* know how much it will use yet. */
#define	MEM_VARIABLE_USAGE 0x8	/* The current work unit is using a */
				/* lot of memory now and if needed could */
				/* use less if restarted. */
#define MEM_WAITING	0x10	/* Set if thread is waiting for another thread */
				/* to stop before returning from set_memory_usage */
char	MEM_FLAGS[MAX_NUM_WORKER_THREADS] = {0};
				/* Flags indicating which threads will be */
				/* affected by a change in memory settings. */
unsigned int MEM_IN_USE[MAX_NUM_WORKER_THREADS] = {0};
				/* Array containing memory in use by each */
				/* worker thread */

#define MEM_RESTART_LOWMEM_ENDS 0x1
				/* Worker needs to restart when */
				/* the LowMemWhileRunning program ends. */
#define MEM_RESTART_MAX_MEM_AVAILABLE 0x2
				/* Worker needs to restart when available memory */
				/* equals maximum memory.  This happens when */
				/* stage 2 is delayed until max memory is available. */
#define MEM_RESTART_MAX_MEM_CHANGE 0x4
				/* Current work unit needs to restart if */
				/* max mem changes.  P-1 may choose different */
				/* bounds because of the change */
#define MEM_RESTART_TOO_MANY_HIGHMEM 0x8
				/* Worker needs to restart because */
				/* MAX_HIGH_MEM_WORKERS exceeded. */
#define MEM_RESTART_MORE_AVAIL 0x10 /* One of the worker's work units did not */
				/* have enough memory to run.  If memory */
				/* becomes available restart the worker. */
#define MEM_RESTART_IF_MORE 0x20 /* The current work unit could use more memory */
				/* and should be restarted if more becomes */
				/* available. */

char	MEM_RESTART_FLAGS[MAX_NUM_WORKER_THREADS] = {0};
unsigned int MEM_RESTART_MIN_AMOUNT[MAX_NUM_WORKER_THREADS] = {0};
unsigned int MEM_RESTART_DESIRED_AMOUNT[MAX_NUM_WORKER_THREADS] = {0};
				/* Only restart if this amount of memory  */
				/* is available */
unsigned int MEM_RESTART_IF_MORE_AMOUNT[MAX_NUM_WORKER_THREADS] = {0};

int	MEM_MUTEX_INITIALIZED = FALSE;
gwmutex	MEM_MUTEX;		/* Lock for accessing mem globals */

/*************************************/
/* Routines used to time code chunks */
/*************************************/

void clear_timers (
	double	*timers,
	int	num_timers)
{
	int	i;
	for (i = 0; i < num_timers; i++) timers[i] = 0.0;
}

void clear_timer (
	double	*timers,
	int	i)
{
	timers[i] = 0.0;
}

void start_timer (
	double	*timers,
	int	i)
{
	if (RDTSC_TIMING < 10) {
		timers[i] -= getHighResTimer ();
	} else if (RDTSC_TIMING > 10 && (CPU_FLAGS & CPU_RDTSC)) {
		uint32_t hi, lo;
		rdtsc (&hi, &lo);
		timers[i] -= (double) hi * 4294967296.0 + lo;
	} else {
		struct _timeb timeval;
		_ftime (&timeval);
		timers[i] -= (double) timeval.time * 1000.0 + timeval.millitm;
	}
}

void end_timer (
	double	*timers,
	int	i)
{
	if (RDTSC_TIMING < 10) {
		timers[i] += getHighResTimer ();
	} else if (RDTSC_TIMING > 10 && (CPU_FLAGS & CPU_RDTSC)) {
		uint32_t hi, lo;
		rdtsc (&hi, &lo);
		timers[i] += (double) hi * 4294967296.0 + lo;
	} else {
		struct _timeb timeval;
		_ftime (&timeval);
		timers[i] += (double) timeval.time * 1000.0 + timeval.millitm;
	}
}

void divide_timer (
	double	*timers,
	int	i,
	int	j)
{
	timers[i] = timers[i] / (double) j;
}

double timer_value (
	double	*timers,
	int	i)
{
	if (RDTSC_TIMING < 10)
		return (timers[i] / getHighResTimerFrequency ());
	else if (RDTSC_TIMING > 10 && (CPU_FLAGS & CPU_RDTSC))
		return (timers[i] / CPU_SPEED / 1000000.0);
	else
		return (timers[i] / 1000.0);
}

#define TIMER_NL	0x1
#define TIMER_CLR	0x2
#define TIMER_OPT_CLR	0x4
#define TIMER_MS	0x8

void print_timer (
	double	*timers,
	int	i,
	char	*buf,
	int	flags)
{
	double	t;

/* The timer could be less than zero if the computer went into hibernation. */
/* Hibernation is where the memory image is saved to disk and the computer */
/* shut off.  Upon power up the memory image is restored but the RDTSC */
/* timestamp counter has been reset to zero. */

	buf += strlen (buf);
	t = timer_value (timers, i);
	if (t < 0.0) {
		strcpy (buf, "Unknown");
		timers[i] = 0.0;
	}

/* Format the timer value in one of several styles */

	else {
		int	style;

		style = IniGetInt (INI_FILE, "TimingOutput", 0);
		if (style == 0) {
			if (flags & TIMER_MS) style = 4;
			else style = 1;
		}

		if (style == 1)
			sprintf (buf, "%.3f sec.", t);
		else if (style == 2)
			sprintf (buf, "%.1f ms.", t * 1000.0);
		else if (style == 3)
			sprintf (buf, "%.2f ms.", t * 1000.0);
		else
			sprintf (buf, "%.3f ms.", t * 1000.0);

		if (RDTSC_TIMING == 12 && (CPU_FLAGS & CPU_RDTSC))
			sprintf (buf+strlen(buf), " (%.0f clocks)", timers[i]);
		else if (RDTSC_TIMING == 2)
			sprintf (buf+strlen(buf), " (%.0f clocks)", t * CPU_SPEED * 1000000.0);
	}

/* Append optional newline */

	if (flags & TIMER_NL) strcat (buf, "\n");

/* Clear the timer */

	if (flags & TIMER_CLR) timers[i] = 0.0;
	if ((flags & TIMER_OPT_CLR) && !CUMULATIVE_TIMING) timers[i] = 0.0;
}

/**************************************************************/
/*    Routines dealing with thread priority and affinity      */
/**************************************************************/

/* Set thread priority and affinity correctly.  Most screen savers run at priority 4. */
/* Most application's run at priority 9 when in foreground, 7 when in */
/* background.  In selecting the proper thread priority I've assumed the */
/* program usually runs in the background. */ 

void SetPriority (
	struct PriorityInfo *info)
{
	int	bind_type, core;
#ifdef BIND_TYPE_1_USED
	int	logical_CPU;
#endif
	char	logical_CPU_string[255];
	char	logical_CPU_substring[255];
	char	buf[255];

/* Call OS-specific routine to set the priority */

	if (IniGetInt (INI_FILE, "EnableSetPriority", 1))
		setOsThreadPriority (PRIORITY);

/* Skip setting affinity if requested by user.  There is no known reason to do this at present. */

	if (! IniGetInt (INI_FILE, "EnableSetAffinity", 1)) return;

/* Skip setting affinity if OS does not support it.  At present time, that is Apple. */

 	if (!OS_CAN_SET_AFFINITY) return;

/* Pick from one of several methodologies to determine affinity setting */

	switch (info->type) {

/* QA affinity.  Let threads run on any CPU. */

	case SET_PRIORITY_QA:
		return;

/* Advanced/Time affinity.  Set affinity to appropriate core. */

	case SET_PRIORITY_TIME:
		bind_type = 0;				// Set affinity to one specific core
		core = info->aux_thread_num / info->time_hyperthreads;
		break;

/* Busy loop on specified CPU core. */

	case SET_PRIORITY_BUSY_LOOP:
		bind_type = 0;				// Set affinity to one specific core
		core = info->busy_loop_cpu;
		break;

/* Torture test affinity.  If we're running the same number of torture */
/* tests as CPUs if the system then set affinity.  Otherwise, let the */
/* threads run on any CPU. */

	case SET_PRIORITY_TORTURE:
		if (info->torture_num_workers * info->torture_threads_per_test == NUM_CPUS * CPU_HYPERTHREADS) {
			bind_type = 0;				// Set affinity to one specific core
			core = (info->worker_num * info->torture_threads_per_test + info->aux_thread_num) / CPU_HYPERTHREADS;
		} else if (info->torture_num_workers * info->torture_threads_per_test == NUM_CPUS) {
			bind_type = 0;				// Set affinity to one specific core
			core = (info->worker_num * info->torture_threads_per_test + info->aux_thread_num);
		} else
			return;					// Run on any core
		break;

/* Benchmarking.  Set affinity to appropriate core. */

	case SET_PRIORITY_BENCHMARKING:
		bind_type = 0;				// Set affinity to one specific core
		core = info->bench_base_cpu_num + info->aux_thread_num / info->bench_hyperthreads;
		break;

/* If user has given an explicit list of logical CPUs to set affinity to, then use that list. */
/* Hopefully, there is no real need to ever use this feature. */

	case SET_PRIORITY_NORMAL_WORK:
		{
			char	section_name[32];
			const char *p;
			sprintf (section_name, "Worker #%d", info->worker_num+1);
			p = IniSectionGetStringRaw (LOCALINI_FILE, section_name, "Affinity");
			if (p != NULL) {
				bind_type = 2;				// Set affinity to a set of logical CPUs
				truncated_strcpy (logical_CPU_string, sizeof (logical_CPU_string), p);
				break;
			}
		}

/* If number of workers equals number of physical cpus then run each */
/* worker on its own physical CPU.  Run auxiliary threads on the same */
/* physical CPU.  This might be advantageous on hyperthreaded CPUs.  User */
/* should be careful to not run more auxiliary threads than available */
/* logical CPUs created by hyperthreading. */ 

		if (NUM_WORKER_THREADS == NUM_CPUS) {
			bind_type = 0;				// Set affinity to a specific physical CPU core
			core = info->worker_num;
			break;
		}

/* If number of workers equals number of logical cpus then run each */
/* worker on its own logical CPU.  If the user is also running */
/* auxiliary threads, then the user has made a really bad decision and a */
/* performance hit will occur running on the same logical CPU. */

		if (NUM_WORKER_THREADS == NUM_CPUS * CPU_HYPERTHREADS) {
			bind_type = 0;				// Set affinity to a specific physical CPU core
			core = info->worker_num / CPU_HYPERTHREADS;
			break;
		}

/* If total num CPUs == num_cpus, then there is an easy path forward */
/* Example: a quad-core with worker 1 using 2 CPUs, and workers 2 & 3 each use 1 CPU --- no problem. */

		{
			int	i, worker_core_count, cores_used_by_lower_workers;
			worker_core_count = cores_used_by_lower_workers = 0;
			for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
				worker_core_count += CORES_PER_TEST[i];
				if (i < info->worker_num) cores_used_by_lower_workers += CORES_PER_TEST[i];
			}

			if (worker_core_count == NUM_CPUS) {
				bind_type = 0;			// Set affinity to a specific physical CPU core
				core = cores_used_by_lower_workers + info->aux_thread_num / info->normal_work_hyperthreads;
				break;
			}

/* If total num cores < num_cpus we will give each thread its own CPU.  There is some anecdotal */
/* evidence that CPU 0 is reserved for interrupt processing (on some OSes), so we leave CPU #0 unused */
/* (hoping hwloc assigns CPU numbers the same way the OS does). */

			if (worker_core_count < (int) NUM_CPUS) {
				bind_type = 0;			// Set affinity to a specific physical CPU core
				core = cores_used_by_lower_workers + info->aux_thread_num / info->normal_work_hyperthreads + 1;
				break;
			}

/* If total num cores is between num_cpus is greater than num_cpus, then what to do? */
/* Our default policy is to throw our hands up in despair and simply run on any CPU. */

			return;
		}

/* No good rule found for setting affinity.  Simply run on any CPU. */

		return;
	}

/* Parse affinity settings specified in the INI file. */
/* We accept several syntaxes in an INI file for a zero-based list of logical CPUs: */
/*	3,6,9		Run main worker thread on logical CPU #3, run two aux threads on logical CPUs #6 & #9 */
/*	3-4,5-6		Run main worker thread on logical CPUs #3 & #4, run aux thread on logical CPUs #5 & #6 */
/*	{3,5,7},{4,6}	Run main worker thread on logical CPUs #3, #5, & #7, run aux thread on logical CPUs #4 & #6 */
/*	(3,5,7),(4,6)	Run main worker thread on logical CPUs #3, #5, & #7, run aux thread on logical CPUs #4 & #6 */
/*	[3,5-7],(4,6)	Run main worker thread on logical CPUs #3, #5, #6, & #7, run aux thread on logical CPUs #4 & #6 */

	if (bind_type == 2) {		// Find the subset of the logical CPU string for this auxillary thread
		int	i;
		char	*p, *start, end_char;
		for (i = 0, p = logical_CPU_string; i <= info->aux_thread_num && *p; i++) {
			while (isspace (*p)) p++;
			start = p;
			end_char = 0;
			if (*p == '{') end_char = '}';
			if (*p == '(') end_char = ')';
			if (*p == '[') end_char = ']';
			if (end_char) {
				p = strchr (start, end_char);
				if (p == NULL) {
					sprintf (buf, "Error parsing affinity string: %s\n", logical_CPU_string);
					OutputStr (info->worker_num, buf);
					return;
				}
				truncated_strcpy_with_len (logical_CPU_substring, sizeof (logical_CPU_substring),
							   start + 1, (int) (p - start - 1));
				if (strchr (p, ',') != NULL) p = strchr (p, ',') + 1;
				else p = p + strlen(p);
			} else {
				p = strchr (start, ',');
				if (p != NULL) {
					truncated_strcpy_with_len (logical_CPU_substring, sizeof (logical_CPU_substring),
								   start, (int) (p - start));
					p++;
				} else {
					truncated_strcpy (logical_CPU_substring, sizeof (logical_CPU_substring), start);
					p = start + strlen (start);
				}
			}
		}
	}

/* Output an informative message */

	if (NUM_CPUS > 1 && info->verbose_flag) {
		if (info->aux_thread_num == 0)
			strcpy (buf, "Setting affinity to run worker on ");
		else
			sprintf (buf, "Setting affinity to run helper thread %d on ", info->aux_thread_num);
		if (bind_type == 0)
			sprintf (buf+strlen(buf), "CPU core #%d\n", core+1);
#ifdef BIND_TYPE_1_USED
		else if (bind_type == 1)
			sprintf (buf+strlen(buf), "logical CPU #%d (zero-based)\n", logical_CPU);
#endif
		else
			sprintf (buf+strlen(buf), "logical CPUs %s (zero-based)\n", logical_CPU_substring);
		OutputStr (info->worker_num, buf);
	}

/* Set affinity for this thread to a specific CPU core */

	if (bind_type == 0) {
		int	num_cores;
		hwloc_obj_t obj;
		num_cores = hwloc_get_nbobjs_by_type (hwloc_topology, HWLOC_OBJ_CORE);
		if (core >= num_cores) {		// This shouldn't happen
			sprintf (buf, "Error setting affinity to core #%d.  There are %d cores.\n", core+1, num_cores);
			OutputStr (info->worker_num, buf);
			core %= num_cores;
		}
		obj = hwloc_get_obj_by_type (hwloc_topology, HWLOC_OBJ_CORE, core);	/* Get proper core */
		if (obj) {
			if (hwloc_set_cpubind (hwloc_topology, obj->cpuset, HWLOC_CPUBIND_THREAD)) { /* Bind thread to all logical CPUs in the core */
				char *str;
				int error = errno;
				hwloc_bitmap_asprintf (&str, obj->cpuset);
				sprintf (buf, "Error setting affinity to cpuset %s: %s\n", str, strerror(error));
				OutputStr (info->worker_num, buf);
				free (str);
			}
		}
	}

/* Set affinity for this thread to a specific logical CPU */

#ifdef BIND_TYPE_1_USED
	else if (bind_type == 1) {
		int	num_logical_CPUs;
		hwloc_obj_t obj;
		num_logical_CPUs = hwloc_get_nbobjs_by_type (hwloc_topology, HWLOC_OBJ_PU);
		if (logical_CPU >= num_logical_CPUs) {		// This shouldn't happen
			sprintf (buf, "Error setting affinity to logical CPU #%d (zero-based).  There are %d logical CPUs.\n", logical_CPU, num_logical_CPUs);
			OutputStr (info->worker_num, buf);
			logical_CPU %= num_logical_CPUs;
		}
		obj = hwloc_get_obj_by_type (hwloc_topology, HWLOC_OBJ_PU, logical_CPU);	/* Get proper logical CPU */
		if (obj) {
			if (hwloc_set_cpubind (hwloc_topology, obj->cpuset, HWLOC_CPUBIND_THREAD)) { /* Bind thread to one logical CPU */
				char *str;
				int error = errno;
				hwloc_bitmap_asprintf (&str, obj->cpuset);
				sprintf (buf, "Error setting affinity to cpuset %s: %s\n", str, strerror(error));
				OutputStr (info->worker_num, buf);
				free (str);
			}
		}
	}
#endif

/* Set affinity for this thread to one or more logical CPUs as specified in the INI file. */

	else {
		char *p, *dash, *comma;
		hwloc_obj_t obj;
		hwloc_bitmap_t cpuset;
		int	start, end;

		cpuset = hwloc_bitmap_alloc ();
		for (p = logical_CPU_substring; ; p = comma+1) {	// Loop through comma-separated list in the logical_CPU_substring
			start = atoi (p);
			dash = strchr (p, '-');
			comma = strchr (p, ',');
			if (dash != NULL && (comma == NULL || dash < comma)) end = atoi (dash+1);
			else end = start;
			while (start <= end) {
				obj = hwloc_get_obj_by_type (hwloc_topology, HWLOC_OBJ_PU, start++);
				if (obj) hwloc_bitmap_or (cpuset, cpuset, obj->cpuset);
			}
			if (comma == NULL) break;
		}
		if (hwloc_set_cpubind (hwloc_topology, cpuset, HWLOC_CPUBIND_THREAD)) {	/* Set affinity to specified logical CPUs */
			char *str;
			int error = errno;
			hwloc_bitmap_asprintf (&str, cpuset);
			sprintf (buf, "Error setting affinity to cpuset %s: %s\n", str, strerror(error));
			OutputStr (info->worker_num, buf);
			free (str);
		}
		hwloc_bitmap_free (cpuset);
	}
}

/* Gwnum thread callback routine */

void SetAuxThreadPriority (int aux_thread_num, int action, void *data)
{
	struct PriorityInfo sp_info;

/* Handle thread start action.  Set the thread priority. */

	if (action == 0) {
		memcpy (&sp_info, data, sizeof (struct PriorityInfo));
		sp_info.aux_thread_num = aux_thread_num;
		SetPriority (&sp_info);
	}

/* Handle thread terminate action.  Remove thread handle from list */
/* of active worker threads. */

	if (action == 1) {
		registerThreadTermination ();
	}
}

/**************************************************************/
/*       Routines and globals dealing with stop codes         */
/*             and the write save files timer                 */
/**************************************************************/

/* This routine checks if the worker thread needs to be stopped for any */
/* reason whatsoever.  If the worker thread should stop, a stop reason */
/* is returned.  The routine is declared EXTERNC becasue it can be called */
/* by the C code in giants that does GCD. */

EXTERNC int stopCheck (
	int	thread_num)	/* Worker thread number */
{

/* Call an OS-specific callback routine.  This gives OSes that poll for */
/* the ESC key a hook.  They can perform any necessary miscellaneous */
/* functions and check for the ESC key to call stop_workers_for_escape. */

	stopCheckCallback (thread_num);

/* If the ESC key was hit, stop processing.  This takes precedence over */
/* all other stop reasons.  This also happens when the program is exiting. */

	if (WORKER_THREADS_STOPPING) return (STOP_ESCAPE);

/* If this request is coming from one of the "special" thread_nums, then */
/* do not check for any more stop codes.  The only time I know this can happen */
/* is when we are running auto-benchmarks using MAIN_THREAD_NUM */

	if (thread_num < 0) return (0);

/* If an important option changed in the GUI, restart all threads. */
/* For example, the user changes the priority of all worker threads. */	

	if (STOP_FOR_RESTART) return (STOP_RESTART);

/* If the during/else time period has ended, stop processing all worker */
/* threads so prime.txt and local.txt can be reprocessed. */

	if (STOP_FOR_REREAD_INI) return (STOP_REREAD_INI);

/* If the memory settings have changed, stop processing affected worker */
/* threads so they can allocate more or less memory. */

	if (STOP_FOR_MEM_CHANGED[thread_num]) {
		STOP_FOR_MEM_CHANGED[thread_num] = 0;
		return (STOP_MEM_CHANGED);
	}

/* If we are on battery power, stop processing all worker */
/* threads until we cease running on the battery. */

	if (STOP_FOR_BATTERY) return (STOP_BATTERY);

/* If we are on battery power, stop processing all worker */
/* threads until we cease running on the battery. */

	if (STOP_FOR_AUTOBENCH) return (STOP_AUTOBENCH);

/* If the thread needs to go do some higher priority work, then stop */
/* processing this work_unit and reprocess the worktodo file. */

	if (STOP_FOR_PRIORITY_WORK[thread_num]) {
		STOP_FOR_PRIORITY_WORK[thread_num] = 0;
		return (STOP_PRIORITY_WORK);
	}

/* If the thread needs to abort the current work unit, then return */
/* that stop code. */

	if (STOP_FOR_ABORT[thread_num]) {
		STOP_FOR_ABORT[thread_num] = 0;
		return (STOP_ABORT);
	}

/* If the thread needs to stop because the user has explicitly stopped (or never */
/* started) this worker, then return the proper stop code. */

	if (!ACTIVE_WORKERS[thread_num]) return (STOP_WORKER);

/* Check if thread should pause because another process is running. */
/* When pause completes, check stop codes again.  We may have been paused */
/* a long time during which other stop timers may have fired. */

	if (STOP_FOR_PAUSE[thread_num] != NULL) {
		return (STOP_PAUSE);
// Do we want to offer INI option to do an immediate pause (next 2 lines) instead???
//		implement_pause (thread_num);
//		return (stopCheck (thread_num));
	}

/* Check if thread should pause because system load is high. */
/* When pause completes, check stop codes again.  We may have been paused */
/* a long time during which other stop timers may have fired. */

	if (STOP_FOR_LOADAVG) {
		STOP_FOR_LOADAVG--;
		implement_loadavg (thread_num);
		return (stopCheck (thread_num));
	}

/* If the thread needs to pause because of the throttle option, then */
/* do so now. */

	if (STOP_FOR_THROTTLE[thread_num]) {
		STOP_FOR_THROTTLE[thread_num] = 0;
		implementThrottle (thread_num);
	}

/* No need to stop */

	return (0);
}

/* Clear flags controlling the stopping of worker threads. */

int stop_events_initialized = FALSE;

void init_stop_code (void)
{
	STOP_FOR_RESTART = FALSE;
	STOP_FOR_REREAD_INI = FALSE;
	STOP_FOR_BATTERY = FALSE;
	STOP_FOR_AUTOBENCH = FALSE;
	STOP_FOR_LOADAVG = 0;
	memset (STOP_FOR_MEM_CHANGED, 0, sizeof (STOP_FOR_MEM_CHANGED));
	memset (STOP_FOR_PRIORITY_WORK, 0, sizeof (STOP_FOR_PRIORITY_WORK));
	memset (STOP_FOR_PAUSE, 0, sizeof (STOP_FOR_PAUSE));
	memset (STOP_FOR_THROTTLE, 0, sizeof (STOP_FOR_THROTTLE));
	memset (STOP_FOR_ABORT, 0, sizeof (STOP_FOR_ABORT));
	memset (WRITE_SAVE_FILES, 0, sizeof (WRITE_SAVE_FILES));
	memset (JACOBI_ERROR_CHECK, 0, sizeof (JACOBI_ERROR_CHECK));
}

/* Signal threads waiting for work to do */

void restart_waiting_workers (
	int	restart_flags)
{
	int	thread_num;
	for (thread_num = 0; thread_num < MAX_NUM_WORKER_THREADS; thread_num++)
		restart_one_waiting_worker (thread_num, restart_flags);
}

void restart_one_waiting_worker (
	int	thread_num,
	int	restart_flags)
{
	if (restart_flags & RESTART_USER_START &&
	    USER_START_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&USER_START_OR_STOP[thread_num]);
	}
	if (restart_flags & RESTART_WORK_AVAILABLE &&
	    WORK_AVAILABLE_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&WORK_AVAILABLE_OR_STOP[thread_num]);
	}
	if (restart_flags & RESTART_END_PAUSE &&
	    END_PAUSE_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&END_PAUSE_OR_STOP[thread_num]);
	}
	if (restart_flags & RESTART_LOADAVG &&
	    END_LOADAVG_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&END_LOADAVG_OR_STOP[thread_num]);
	}
	if (restart_flags & RESTART_MEM_WAIT &&
	    MEM_WAIT_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&MEM_WAIT_OR_STOP[thread_num]);
	}
	if (restart_flags & RESTART_BATTERY &&
	    OFF_BATTERY_OR_STOP_INITIALIZED[thread_num]) {
		gwevent_signal (&OFF_BATTERY_OR_STOP[thread_num]);
	}
}

/* Set flags so that worker threads will stop due to ESC key being pressed. */

void stop_workers_for_escape (void)
{
	if (WORKER_THREADS_ACTIVE) {
		OutputStr (MAIN_THREAD_NUM, "Stopping all worker threads.\n");
		WORKER_THREADS_STOPPING = TRUE;
		restart_waiting_workers (RESTART_ALL);
		raiseAllWorkerThreadPriority ();
	}
}

/* Set flag so that all worker threads stop and restart because an */
/* important INI option changed (like thread priority).  This routine only */
/* restarts "genuine" work threads - not benchmarking and torture test */
/* work threads. */

void stop_workers_for_restart (void)
{
	if (WORKER_THREADS_ACTIVE &&
	    LAUNCH_TYPE == LD_CONTINUE &&
	    ! STOP_FOR_RESTART) {
		OutputStr (MAIN_THREAD_NUM, "Restarting all worker threads.\n");
		STOP_FOR_RESTART = TRUE;
		restart_waiting_workers (RESTART_ALL);
		if (NUM_WORKER_THREADS > WORKER_THREADS_ACTIVE)
			create_worker_windows (NUM_WORKER_THREADS);
	}
}

/* Set flag so that all worker threads stop and restart because an */
/* important INI option changed (like thread priority). */

void stop_workers_for_add_files (void)
{
	if (WORKER_THREADS_ACTIVE && ! STOP_FOR_RESTART) {
		OutputStr (MAIN_THREAD_NUM, "Restarting all worker threads to process .add file.\n");
		STOP_FOR_RESTART = TRUE;
		restart_waiting_workers (RESTART_ALL);
	}
}

/* Set flag so that worker threads will stop due to Time= end time being */
/* reached.  We need to stop all worker threads, reprocess prime.ini, and */
/* restart the worker threads. */

void stop_workers_for_reread_ini (void)
{
	if (WORKER_THREADS_ACTIVE && ! STOP_FOR_REREAD_INI) {
		OutputStr (MAIN_THREAD_NUM, "Restarting all worker threads with new timed INI settings.\n");
		STOP_FOR_REREAD_INI = TRUE;
		restart_waiting_workers (RESTART_ALL);
	}
}

/* Set flags so that worker threads will stop due to day/night memory */
/* changeover. */

void stop_worker_for_mem_changed (
	int	thread_num)
{
	if (WORKER_THREADS_ACTIVE && ! STOP_FOR_MEM_CHANGED[thread_num]) {
		OutputStr (thread_num, "Restarting worker with new memory settings.\n");
		MEM_FLAGS[thread_num] |= MEM_RESTARTING;
		STOP_FOR_MEM_CHANGED[thread_num] = 1;
		restart_one_waiting_worker (thread_num, RESTART_ALL);
	}
}

/* Set flag so that worker thread will stop to do priority work. */

void stop_worker_for_advanced_test (
	int	thread_num)
{
	if (WORKER_THREADS_ACTIVE && ! STOP_FOR_PRIORITY_WORK[thread_num]) {
		OutputStr (thread_num, "Restarting worker to do LL test.\n");
		STOP_FOR_PRIORITY_WORK[thread_num] = 1;
	}
}

/* Set flag so that worker thread will stop to do priority work. */

void stop_worker_for_priority_work (
	int	thread_num)
{
	if (WORKER_THREADS_ACTIVE && ! STOP_FOR_PRIORITY_WORK[thread_num]) {
		OutputStr (thread_num, "Restarting worker to do factoring prior to LL test.\n");
		STOP_FOR_PRIORITY_WORK[thread_num] = 1;
	}
}

/* Set flags so that worker threads will stop for throttling. */

void stop_workers_for_throttle (void)
{
	if (WORKER_THREADS_ACTIVE)
		memset (STOP_FOR_THROTTLE, 1, sizeof (STOP_FOR_THROTTLE));
}

/* Set flags so that worker thread will abort processing its current */
/* work unit.  There are many reasons to do this: unreserve, factor found */
/* in another thread, server request, etc. */

void stop_worker_for_abort (
	int	thread_num)
{
	if (WORKER_THREADS_ACTIVE)
		STOP_FOR_ABORT[thread_num] = 1;
}

/* Start save files timer */

void start_save_files_timer ()
{
	add_timed_event (TE_SAVE_FILES, DISK_WRITE_TIME * 60);
}

/* Stop save files timer */

void stop_save_files_timer ()
{
	delete_timed_event (TE_SAVE_FILES);
}

/* Set flags so that worker threads will write save files */
/* at next convenient opportunity. */

void saveFilesTimer ()
{
	memset (WRITE_SAVE_FILES, 1, sizeof (WRITE_SAVE_FILES));
	start_save_files_timer ();
}

/* Return TRUE if it is time to write a save file. */

int testSaveFilesFlag (
	int	thread_num)
{
	if (WRITE_SAVE_FILES[thread_num]) {
		WRITE_SAVE_FILES[thread_num] = 0;
		return (TRUE);
	}
	return (FALSE);
}

/* Start Jacobi error check timer */

void start_Jacobi_timer ()
{
	add_timed_event (TE_JACOBI, JACOBI_TIME * 60 * 60);
}

/* Stop Jacobi error check timer */

void stop_Jacobi_timer ()
{
	delete_timed_event (TE_JACOBI);
}

/* Set flags so that worker threads will execute a Jacobi error check */
/* at next convenient opportunity. */

void JacobiTimer ()
{
	memset (JACOBI_ERROR_CHECK, 1, sizeof (JACOBI_ERROR_CHECK));
	start_Jacobi_timer ();
}

/* Return TRUE if it is time to do a Jacobi error check. */

int testJacobiFlag (
	int	thread_num)
{
	if (JACOBI_ERROR_CHECK[thread_num]) {
		JACOBI_ERROR_CHECK[thread_num] = 0;
		return (TRUE);
	}
	return (FALSE);
}

/**************************************************************/
/*      Routines dealing with Day/Night memory settings       */
/**************************************************************/

/* Read the Memory settings from INI file */

void read_mem_info (void)
{
	const char *p;
	int	tnum;
	unsigned int seconds, seconds_until_reread;

/* Initalize the memory mutex and other memory related events */

	if (!MEM_MUTEX_INITIALIZED) {
		MEM_MUTEX_INITIALIZED = 1;
		gwmutex_init (&MEM_MUTEX);
	}

/* Lock just in case memory routines are accessing this data */

	gwmutex_lock (&MEM_MUTEX);

/* Kill the timer that triggers rereading the memory info */

	delete_timed_event (TE_MEM_CHANGE);

/* Read and parse the Memory data from the INI file */

	seconds_until_reread = 0;
	AVAIL_MEM = IniGetTimedInt (LOCALINI_FILE, "Memory", 8, &seconds);
	if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
		seconds_until_reread = seconds;
	for (tnum = 0; tnum < (int) MAX_NUM_WORKER_THREADS; tnum++) {
		char	section_name[32];
		sprintf (section_name, "Worker #%d", tnum+1);
		AVAIL_MEM_PER_WORKER[tnum] = IniSectionGetTimedInt (LOCALINI_FILE, section_name, "Memory", AVAIL_MEM, &seconds);
		if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
			seconds_until_reread = seconds;
	}

/* Compute the maximum memory setting.  If not found, assume 8MB. */

	MAX_MEM = 8;
	p = IniSectionGetStringRaw (LOCALINI_FILE, NULL, "Memory");
	if (p != NULL) for ( ; ; ) {
		unsigned long mem = atol (p);
		if (mem > MAX_MEM) MAX_MEM = mem;
		p = strstr (p, " else ");
		if (p == NULL) break;
		p = p + 6;
	}

/* Get the maximum number of workers that can use lots of memory */
/* Default is AVAIL_MEM / 200MB rounded off. */

	MAX_HIGH_MEM_WORKERS = IniGetTimedInt (LOCALINI_FILE, "MaxHighMemWorkers",
					       (AVAIL_MEM + 100) / 200, &seconds);
	if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
		seconds_until_reread = seconds;
	if (MAX_HIGH_MEM_WORKERS < 1) MAX_HIGH_MEM_WORKERS = 1;

/* Add the event that fires when the memory settings expire. */

	if (seconds_until_reread)
		add_timed_event (TE_MEM_CHANGE, seconds_until_reread);

/* Unlock */

	gwmutex_unlock (&MEM_MUTEX);
}

/* This routine initializes mem_changed globals.  It must be called prior */
/* to launching the worker threads. */

void init_mem_state (void)
{
	int	i;

/* Clear flags saying thread is affected by changes in the memory settings. */
/* Assume each worker thread will use a default amount of memory. */

	for (i = 0; i < MAX_NUM_WORKER_THREADS; i++) {
		MEM_FLAGS[i] = MEM_USAGE_NOT_SET;
		MEM_IN_USE[i] = DEFAULT_MEM_USAGE;
		MEM_RESTART_FLAGS[i] = 0;
	}
}

/* Clear flags that keep track if the thread needs restarting */
/* on available memory changes. */

void clear_memory_restart_flags (
	int	thread_num)
{
	MEM_RESTART_FLAGS[thread_num] = 0;
}

/* Set thread to default memory usage.  For now, this is 24MB -- roughly */
/* the amount of memory used by LL test using a 2.5M FFT. */

void set_default_memory_usage (
	int	thread_num)
{
	MEM_FLAGS[thread_num] = MEM_USAGE_NOT_SET;
	MEM_IN_USE[thread_num] = DEFAULT_MEM_USAGE;

/* Clear restart flags that only apply to current work unit as opposed to */
/* most flags which are not reset until primeContinue reprocesses the worker's */
/* complete list of work. */

	MEM_RESTART_FLAGS[thread_num] &= ~MEM_RESTART_MAX_MEM_CHANGE;
	MEM_RESTART_FLAGS[thread_num] &= ~MEM_RESTART_IF_MORE;
}

/* Set flag that restarts worker if max mem changes. */
/* Needed for Pfactor so that we can compute new bounds */
/* should max mem change. */

void set_restart_if_max_memory_change (
	int	thread_num)
{
	MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_MAX_MEM_CHANGE;
}

/* Set flag that restarts worker if max mem changes. */
/* Needed for Pfactor so that we can compute new bounds */
/* should max mem change. */

void clear_restart_if_max_memory_change (
	int	thread_num)
{
	MEM_RESTART_FLAGS[thread_num] &= ~MEM_RESTART_MAX_MEM_CHANGE;
}

/* Set flag that restarts current work unit if more memory */
/* becmes available.  Used when stage 2 got far less memory than */
/* it wanted and significantly more memory whould speed up stage 2. */

void set_restart_if_more_memory_available (
	int	thread_num,
	unsigned int memory)		/* Memory needed for a restart in MB */
{
	MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_IF_MORE;
	MEM_RESTART_IF_MORE_AMOUNT[thread_num] = memory;
}

/* If the caller of avail_mem wasn't happy with the amount of memory */
/* returned, he can call this routine to set flags so that worker will be */
/* restarted when more memory becomes available. */

int avail_mem_not_sufficient (
	int	thread_num,
	unsigned long min_memory,	/* Minumum memory in MB */
	unsigned long desired_memory)	/* Desired memory in MB */
{
	OutputStr (thread_num, "Other workers are using lots of memory now.\n");
	if (MEM_RESTART_FLAGS[thread_num] & MEM_RESTART_MORE_AVAIL) {
		if (min_memory < MEM_RESTART_MIN_AMOUNT[thread_num])
			MEM_RESTART_MIN_AMOUNT[thread_num] = min_memory;
		if (desired_memory < MEM_RESTART_DESIRED_AMOUNT[thread_num])
			MEM_RESTART_DESIRED_AMOUNT[thread_num] = desired_memory;
	} else {
		MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_MORE_AVAIL;
		MEM_RESTART_MIN_AMOUNT[thread_num] = min_memory;
		MEM_RESTART_DESIRED_AMOUNT[thread_num] = desired_memory;
	}
	return (STOP_NOT_ENOUGH_MEM);
}

/* Internal routine that returns TRUE if other threads are using lots of */
/* the available memory.  We use this to delay ECM and P-1 stage 2 while other */
/* stage 2's are running. */

int are_threads_using_lots_of_memory (
	int	thread_num)
{
	int	max_high_mem, i;

/* Get the user configurable count of workers that are allowed to use */
/* lots of memory.  If this equals the number of workers (default) then */
/* there is no need to scan the workers */

	max_high_mem = MAX_HIGH_MEM_WORKERS;
	if (max_high_mem >= (int) NUM_WORKER_THREADS) return (FALSE);

/* If there are enough threads with variable memory usage, then return TRUE. */
/* To guard against an ECM stage 2 that really isn't using a whole lot of */
/* memory, also require the thread to be using 50MB. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++)
		if (i != thread_num &&
		    (MEM_FLAGS[i] & MEM_VARIABLE_USAGE ||
		     MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE) &&
		    MEM_IN_USE[i] >= 50) {
			max_high_mem--;
			if (max_high_mem == 0) return (TRUE);
		}
	return (FALSE);
}

/* Each worker thread tells us how much memory it will be using.  This may */
/* cause other worker threads to restart if they are using more than their */
/* fair share. */
/* Variable usage callers must examine the return code!  During startup */
/* all threads may not have determined their memory needs.  This routine */
/* returns TRUE if caller should recalculate the amount of memory available */
/* for use because we previously overestimated the amount of memory available */
/* to the thread. */

int set_memory_usage (
	int	thread_num,
	int	flags,		/* Valid values are MEM_VARIABLE_USAGE */
				/* and MEM_USAGE_NOT_SET */
	unsigned long memory)	/* Memory in use (in MB) */
{
	int	i, best_thread, worst_thread, all_threads_set;
	unsigned long mem_usage;

/* Obtain lock before accessing memory global variables */

	gwmutex_lock (&MEM_MUTEX);

/* Set or clear flag indicating thread is executing code that can choose a */
/* different amount of memory to use. */

	if (flags & MEM_VARIABLE_USAGE)
		MEM_FLAGS[thread_num] |= MEM_VARIABLE_USAGE;
	else
		MEM_FLAGS[thread_num] &= ~MEM_VARIABLE_USAGE;
	MEM_FLAGS[thread_num] &= ~MEM_WILL_BE_VARIABLE_USAGE;

/* Set flag indicating we are guessing how much memory this thread */
/* will use because the thread has not started its work unit. */

	if (flags & MEM_USAGE_NOT_SET)
		MEM_FLAGS[thread_num] |= MEM_USAGE_NOT_SET;
	else
		MEM_FLAGS[thread_num] &= ~MEM_USAGE_NOT_SET;
	MEM_FLAGS[thread_num] &= ~MEM_RESTARTING;

/* Record the amount of memory being used */

	MEM_IN_USE[thread_num] = memory;

/* Sum up the amount of memory used by all threads.  In case we've allocated */
/* too much memory, select a variable thread to restart.  We do this to make */
/* the thread reduce its memory usage so that the other threads will be OK. */
/* We'll restart the variable thread using the most memory. */

	mem_usage = 0;
	worst_thread = -1;
	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		mem_usage += MEM_IN_USE[i];
		if ((MEM_FLAGS[i] & MEM_VARIABLE_USAGE ||
		     MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE) &&
		    (worst_thread == -1 ||
		     MEM_IN_USE[i] > MEM_IN_USE[worst_thread]))
			worst_thread = i;
	}

/* If we have allocated more than the maximum allowable, then stop a */
/* thread to free up some memory.  We also make sure we are using significantly */
/* more memory than we should be so that minor fluctuations in memory */
/* usage by the fixed threads do not cause needless restarts.  The 32MB */
/* threshold is arbitrary. */

	if (mem_usage > AVAIL_MEM + 32) {

/* If the current thread is the worst thread (should only happen if there has */
/* been a wild change in other thread's memory usage between the call to */
/* avail_mem and the call to set_memory_usage), then return to caller and */
/* tell it to try again.  WARNING:  this could cause an infinite */
/* loop if caller misbehaves and tries to use the same amount of memory. */

		if (worst_thread == thread_num) {
			set_default_memory_usage (thread_num);
			gwmutex_unlock (&MEM_MUTEX);
			return (TRUE);
		}

/* If we found a worst thread and that thread has actually allocated */
/* memory (MEM_VARIABLE_USAGE), as opposed to being in the process of */
/* figuring out its memory needs (MEM_WILL_BE_VARIABLE_USAGE), then */
/* stop the offending thread. */

		if (worst_thread >= 0 && MEM_FLAGS[worst_thread] & MEM_VARIABLE_USAGE) {
			stop_worker_for_mem_changed (worst_thread);

/* Wait for the stop to take effect so that we don't briefly over-allocate memory. */

			MEM_FLAGS[thread_num] |= MEM_WAITING;
			gwmutex_unlock (&MEM_MUTEX);
			gwevent_init (&MEM_WAIT_OR_STOP[thread_num]);
			gwevent_reset (&MEM_WAIT_OR_STOP[thread_num]);
			MEM_WAIT_OR_STOP_INITIALIZED[thread_num] = 1;
			gwevent_wait (&MEM_WAIT_OR_STOP[thread_num], 20);
			MEM_WAIT_OR_STOP_INITIALIZED[thread_num] = 0;
			gwevent_destroy (&MEM_WAIT_OR_STOP[thread_num]);
			gwmutex_lock (&MEM_MUTEX);
			MEM_FLAGS[thread_num] &= ~MEM_WAITING;
		}
	}

/* See if all fixed usage threads have set their memory usage */

	all_threads_set = TRUE;
	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE) continue;
		if (MEM_FLAGS[i] & MEM_USAGE_NOT_SET ||
		    MEM_FLAGS[i] & MEM_RESTARTING) {
			all_threads_set = FALSE;
			break;
		}
	}

/* If all fixed usage threads have called this routine setting their memory */
/* usage, then signal an event to wake up one of variable usage workers */
/* that is waiting for all fixed usage workers to compute their memory usage. */

	if (all_threads_set) {
		best_thread = -1;
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
			if (MEM_FLAGS[i] & MEM_WAITING) {
				best_thread = i;
				break;
			}
			if (MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE &&
			    (best_thread == -1 ||
			     MEM_IN_USE[i] < MEM_IN_USE[best_thread]))
				best_thread = i;
		}
		if (best_thread >= 0) {
			restart_one_waiting_worker (best_thread, RESTART_MEM_WAIT);
			all_threads_set = FALSE;
		}
	}

/* If a worker is waiting for a reduction in the number of workers */
/* using lots of memory, then check to see if it can run now. */
/* The 32 is an arbitrary figure that makes sure a significant amount */
/* of new memory is available before restarting worker threads. */
/* Be careful subtracting from AVAIL_MEM.  Since it is an unsigned long */
/* if it goes negative it will become a large positive value instead */	

	if (all_threads_set && AVAIL_MEM > mem_usage + 32 ) {
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
			if (! (MEM_RESTART_FLAGS[i] & MEM_RESTART_TOO_MANY_HIGHMEM)) continue;
			if (are_threads_using_lots_of_memory (i)) continue;
			stop_worker_for_mem_changed (i);
			all_threads_set = FALSE;
			break;
		}
	}

/* If all fixed and variable usage threads have set their memory usage, */
/* then if we have enough free memory restart a work unit that could use */
/* more memory. */

	if (all_threads_set && AVAIL_MEM > mem_usage + 32) {
		best_thread = -1;
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
			if (MEM_RESTART_FLAGS[i] & MEM_RESTART_IF_MORE &&
			    MEM_RESTART_IF_MORE_AMOUNT[i] < AVAIL_MEM - mem_usage)
				best_thread = i;
		}
		if (best_thread >= 0) {
			stop_worker_for_mem_changed (best_thread);
			all_threads_set = FALSE;
		}
	}
		
/* If all fixed and variable usage threads have set their memory usage, */
/* then if we have enough free memory restart a thread that couldn't */
/* run a work unit due to lack of available memory. */

	if (all_threads_set && AVAIL_MEM > mem_usage + 32) {
		best_thread = -1;
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
			if (MEM_RESTART_FLAGS[i] & MEM_RESTART_MORE_AVAIL &&
			    MEM_RESTART_MIN_AMOUNT[i] < AVAIL_MEM - mem_usage)
				best_thread = i;
		}
		if (best_thread >= 0) {
			stop_worker_for_mem_changed (best_thread);
			all_threads_set = FALSE;
		}
	}

/* All done */

	gwmutex_unlock (&MEM_MUTEX);
	return (FALSE);
}

/* Return maximum memory (in MB) that will ever be available for a variable usage thread. */

unsigned long max_mem (
	int	thread_num)
{
	char	section_name[32];
	unsigned long memory;
	const char *p;

/* Compute the maximum memory setting for this thread.  If not found, return the global max memory. */

	sprintf (section_name, "Worker #%d", thread_num+1);
	p = IniSectionGetStringRaw (LOCALINI_FILE, section_name, "Memory");
	if (p == NULL) return (MAX_MEM);

	memory = 0;
	for ( ; ; ) {
		unsigned long temp = atol (p);
		if (temp > memory) memory = temp;
		p = strstr (p, " else ");
		if (p == NULL) break;
		p = p + 6;
	}

/* Return the lesser of the global max memory and the thread's max memory */

	if (memory < MAX_MEM) return (memory);
	return (MAX_MEM);
}

/* Return memory (in MB) now available for a variable usage thread. */
/* This routine takes into account the memory used by other worker threads. */
/* NOTE: caller is expected to have called are_threads_using_lots_of_memory */
/* to make sure too many workers don't become high memory users. */

int avail_mem (
	int	thread_num,
	unsigned long minimum_memory,	/* If this much memory (in MB) */
					/* can be returned without restarting other */
					/* workers, then do so */
	unsigned long desired_memory,	/* If this much memory (in MB) */
					/* can be returned without restarting other */
					/* workers, then do so */
	unsigned int *memory)		/* Returned available memory, in MB */
{
	int	i, fixed_threads[MAX_NUM_WORKER_THREADS];
	unsigned long fixed_usage, variable_usage, num_variable_threads, avail, diff;

/* Check if we are in a period of forced low memory usage */

	if (is_LowMemWhileRunning_active (thread_num)) {
		MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_LOWMEM_ENDS;
		return (STOP_NOT_ENOUGH_MEM);
	}

/* Check if we are only supposed to run high memory workers when the maximum */
/* amount memory is available. */

	if (IniGetInt (INI_FILE, "OnlyRunStage2WithMaxMemory", 0) &&
	    AVAIL_MEM != MAX_MEM) {
		OutputStr (thread_num, "Waiting for maximum available memory to run stage 2.\n");
		MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_MAX_MEM_AVAILABLE;
		return (STOP_NOT_ENOUGH_MEM);
	}

/* Check if we must wait for more memory to become available.  This */
/* happens when we reach the maximum allowable number of threads using a lot */
/* of memory. */

	if (are_threads_using_lots_of_memory (thread_num)) {
		OutputStr (thread_num, "Exceeded limit on number of workers that can use lots of memory.\n");
		MEM_RESTART_FLAGS[thread_num] |= MEM_RESTART_TOO_MANY_HIGHMEM;
		return (STOP_NOT_ENOUGH_MEM);
	}

/* Obtain lock before accessing memory global variables */

	gwmutex_lock (&MEM_MUTEX);

/* Set flag saying this will be a variable usage thread.  Remember the */
/* "good enough" value as it will be helpful in determining the best */
/* value this routine should return (for this thread and other threads) */

	MEM_FLAGS[thread_num] |= MEM_WILL_BE_VARIABLE_USAGE;
	MEM_IN_USE[thread_num] = desired_memory;

/* If any workers have not yet set their memory usage, then wait for them */
/* to do so.  This allows us to accurately gauge how much fixed memory */
/* is consumed and how many variable usage workers there are. */
/* Just in case we wake up from the timeout (should rarely happen), we try*/
/* to stagger the timeouts by adding the thread number. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (i == thread_num) continue;
		if (MEM_FLAGS[i] & MEM_USAGE_NOT_SET ||
		    MEM_FLAGS[i] & MEM_RESTARTING) {
			gwmutex_unlock (&MEM_MUTEX);
			gwevent_init (&MEM_WAIT_OR_STOP[thread_num]);
			gwevent_reset (&MEM_WAIT_OR_STOP[thread_num]);
			MEM_WAIT_OR_STOP_INITIALIZED[thread_num] = 1;
			gwevent_wait (&MEM_WAIT_OR_STOP[thread_num], 20 + thread_num);
			MEM_WAIT_OR_STOP_INITIALIZED[thread_num] = 0;
			gwevent_destroy (&MEM_WAIT_OR_STOP[thread_num]);
			gwmutex_lock (&MEM_MUTEX);
		}
	}

/* Sum up the amount of memory used by threads that cannot adjust their */
/* memory usage.  Also count how many threads (including this one) can */
/* adjust their memory usage. */

	fixed_usage = 0;
	variable_usage = 0;
	num_variable_threads = 0;
	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (MEM_FLAGS[i] & MEM_VARIABLE_USAGE ||
		    MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE) {
			num_variable_threads++;
			variable_usage += MEM_IN_USE[i];
			fixed_threads[i] = FALSE;
		} else {
			fixed_usage += MEM_IN_USE[i];
			fixed_threads[i] = TRUE;
		}
	}

/* We can now calculate how much memory is available for the threads */
/* that are using a variable amount of memory.  */

	avail = (AVAIL_MEM > fixed_usage) ? AVAIL_MEM - fixed_usage : 0;
	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		unsigned long avail_per_worker;
		if (i == thread_num) continue;
		if (fixed_threads[i]) continue;
		avail_per_worker = avail / num_variable_threads;

/* If any variable threads are either using less memory than they are */
/* allowed to use or all variable threads can fit in available memory, */
/* then treat this worker like a fixed memory user. */

		if (MEM_FLAGS[i] & MEM_VARIABLE_USAGE &&
		    (MEM_IN_USE[i] < avail_per_worker ||
		     fixed_usage + variable_usage <= AVAIL_MEM)) {
			avail -= MEM_IN_USE[i];
			fixed_threads[i] = TRUE;
			num_variable_threads--;
			i = -1;	    /* Restart loop */
			continue;
		}

/* If any variable thread is prohibited from using its full share */
/* of the remaining available pool, then distribute the excess among */
/* the other variable usage threads. */

		if (MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE &&
		    AVAIL_MEM_PER_WORKER[i] < avail_per_worker) {
			avail -= AVAIL_MEM_PER_WORKER[i];
			fixed_threads[i] = TRUE;
			num_variable_threads--;
			i = -1;	    /* Restart loop */
			continue;
		}
	}
	avail = avail / num_variable_threads;

/* Free lock after accessing memory global variables */

	gwmutex_unlock (&MEM_MUTEX);

/* If there weren't enough memory available, try again later */

	if (avail < minimum_memory)
		return (avail_mem_not_sufficient (thread_num, minimum_memory, desired_memory));

/* Return the amount of memory this thread can use.  If all variable */
/* threads can obtain their desired memory, then distribute the excess */
/* among all the variable threads.  Otherwise, return my pro-rata share */
/* of variable memory, any overcommitted workers will be restarted once this */
/* thread calls set_memory_usage letting us know how much of the available */
/* memory it actually used. */

	if (fixed_usage + variable_usage <= AVAIL_MEM)
		*memory = desired_memory +
			  (AVAIL_MEM - (fixed_usage + variable_usage)) / num_variable_threads;
	else
		*memory = avail;

/* If memory exceeds this worker's maximum, then only return */
/* this worker's maximum. */

	if (*memory > AVAIL_MEM_PER_WORKER[thread_num])
		*memory = AVAIL_MEM_PER_WORKER[thread_num];

/* As a first approximation, mark the work unit as available for restart */
/* if more memory is available whenever we are near minimum_memory.  The caller */
/* can override our guess if he so desires */	

	diff = desired_memory - minimum_memory;
	if (*memory <= minimum_memory + diff / 4)
		set_restart_if_more_memory_available (thread_num, diff / 4);
	else if (*memory <= minimum_memory + diff / 2)
		set_restart_if_more_memory_available (thread_num, diff / 2);

/* Return clean stop code */

	return (0);
}

/* Routine to notify all worker threads the day/night memory settings */
/* have changed.  This is called when the memory change timer fires OR */
/* when memory settings are changed by the GUI. */

void mem_settings_have_changed (void)
{
	unsigned int old_avail_mem, old_max_mem;
	int	tnum;

/* Recompute the available memory and restart the memory changed timer */

	old_avail_mem = AVAIL_MEM;
	old_max_mem = MAX_MEM;
	read_mem_info ();

/* If the worker threads are not active then no workers need restarting */

	if (! WORKER_THREADS_ACTIVE) return;

/* If maximum memory has changed see which threads need restarting. */
/* Those threads that are in stage 1 of pfactor work will want to compute */
/* new bounds. */

	if (MAX_MEM != old_max_mem)
		for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++)
			if (MEM_RESTART_FLAGS[tnum] & MEM_RESTART_MAX_MEM_CHANGE)
				stop_worker_for_mem_changed (tnum);

/* If available memory is now equal to maximum memory see which threads */
/* need restarting. Those threads that postponed work because they only */
/* run during memory need restarting. */

	if (AVAIL_MEM == MAX_MEM)
		for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++)
			if (MEM_RESTART_FLAGS[tnum] & MEM_RESTART_MAX_MEM_AVAILABLE)
				stop_worker_for_mem_changed (tnum);

/* If available memory has increased we may pick a thread to restart. */
/* Those threads that postponed work because there wasn't enough memory */
/* need restarting. */

	if (AVAIL_MEM > old_avail_mem)
		for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++)
			if (MEM_RESTART_FLAGS[tnum] & MEM_RESTART_MORE_AVAIL)
				stop_worker_for_mem_changed (tnum);

/* If any worker now exceeds (by 10MB) the per-worker maximum, then restart. */

	for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++)
		if (MEM_FLAGS[tnum] & MEM_VARIABLE_USAGE &&
		    MEM_IN_USE[tnum] > AVAIL_MEM_PER_WORKER[tnum] + 10)
			stop_worker_for_mem_changed (tnum);

/* If available memory has decreased we may pick a thread to restart. */
/* If total memory in use is greater than the new available, then pick */
/* one of the variable threads to restart.  Note that if any threads */
/* haven't yet set their memory usage, then when they do set their memory */
/* usage this overcommitment will be sorted out then. */

	if (AVAIL_MEM < old_avail_mem) {
		unsigned long mem_usage;
		int	worst_thread;

		mem_usage = 0;
		worst_thread = -1;
		for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++) {
			mem_usage += MEM_IN_USE[tnum];
			if (MEM_FLAGS[tnum] & MEM_USAGE_NOT_SET ||
			    MEM_FLAGS[tnum] & MEM_RESTARTING ||
			    MEM_FLAGS[tnum] & MEM_WILL_BE_VARIABLE_USAGE) {
				worst_thread = -1;
				break;
			}
			if (MEM_FLAGS[tnum] & MEM_VARIABLE_USAGE &&
			    (worst_thread == -1 ||
			     MEM_IN_USE[tnum] > MEM_IN_USE[worst_thread]))
				worst_thread = tnum;
		}
		if (mem_usage > AVAIL_MEM + 32 && worst_thread != -1)
			stop_worker_for_mem_changed (worst_thread);
	}
}

/* Routine to force any workers that are using lots of memory to stop */
/* and restart.  This happens when LowMemWhileRunnning is activated. */

void stop_high_memory_workers (void)
{
	int	i;

/* If the worker threads are not active then no workers need restarting */

	if (! WORKER_THREADS_ACTIVE) return;

/* Obtain lock before accessing memory global variables */

	gwmutex_lock (&MEM_MUTEX);

/* Look for workers marked with variable usage */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (MEM_FLAGS[i] & MEM_VARIABLE_USAGE ||
		    MEM_FLAGS[i] & MEM_WILL_BE_VARIABLE_USAGE)
			stop_worker_for_mem_changed (i);
	}

/* All done */

	gwmutex_unlock (&MEM_MUTEX);
}

/* Routine to restart workers that were stopped due to LowMemWhileRunning */

void restart_high_memory_workers (void)
{
	int	tnum;

/* If the worker threads are not active then no workers need restarting */

	if (! WORKER_THREADS_ACTIVE) return;

/* Restart the workers affected by LowMemWhileRunning */

	for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++)
		if (MEM_RESTART_FLAGS[tnum] & MEM_RESTART_LOWMEM_ENDS) {
			MEM_RESTART_FLAGS[tnum] &= ~MEM_RESTART_LOWMEM_ENDS;
			stop_worker_for_mem_changed (tnum);
		}
}


/**************************************************************/
/*           Routines dealing running on battery              */
/**************************************************************/

void start_battery_timer (void)
{
	if (RUN_ON_BATTERY) return;
	add_timed_event (TE_BATTERY_CHECK, TE_BATTERY_CHECK_FREQ);
}

void stop_battery_timer (void)
{
	delete_timed_event (TE_BATTERY_CHECK);
}

/* This routine is called if the user changes the RUN_ON_BATTERY setting */
/* from the GUI or it is changed by talking to the server. */

void run_on_battery_changed (void)
{
	if (WORKER_THREADS_ACTIVE) {
		stop_battery_timer ();
		start_battery_timer ();
	}
}

void test_battery (void)
{
	if (OnBattery ()) STOP_FOR_BATTERY = TRUE;
	else if (STOP_FOR_BATTERY) {
		STOP_FOR_BATTERY = FALSE;
		restart_waiting_workers (RESTART_BATTERY);
	}
}

/* Stopping while on battery power, restart thread only when AC power */
/* is restored. */

void implement_stop_battery (
	int	thread_num)
{

/* Output message, change title and icon */

	title (thread_num, "Battery Pause");
	OutputStr (thread_num, "Worker stopped while on battery power.\n");
	ChangeIcon (thread_num, IDLE_ICON);

/* Wait for AC power.  In case AC power was restored before we got here */
/* (LL save files can take some time to generate), do not wait.  The timer */
/* that would trigger the wait event has already fired.  */

	if (OnBattery ()) {
		gwevent_init (&OFF_BATTERY_OR_STOP[thread_num]);
		gwevent_reset (&OFF_BATTERY_OR_STOP[thread_num]);
		OFF_BATTERY_OR_STOP_INITIALIZED[thread_num] = 1;
		gwevent_wait (&OFF_BATTERY_OR_STOP[thread_num], 0);
		OFF_BATTERY_OR_STOP_INITIALIZED[thread_num] = 0;
		gwevent_destroy (&OFF_BATTERY_OR_STOP[thread_num]);
	}

/* Output message, change title and icon */

	title (thread_num, "Working");
	OutputStr (thread_num, "AC power restored, restarting worker.\n");
	ChangeIcon (thread_num, WORKING_ICON);
}

/**************************************************************/
/*              Routine dealing auto-benchmark                */
/**************************************************************/

/* Stopping while on battery power, restart thread only when AC power */
/* is restored. */

void implement_stop_autobench (
	int	thread_num)
{

/* Output message, change title and icon */

	title (thread_num, "Benchmark Pause");
	OutputStr (thread_num, "Worker stopped while running needed benchmarks.\n");
	ChangeIcon (thread_num, IDLE_ICON);

/* Wait for benchmarks to complete */

	gwevent_wait (&AUTOBENCH_EVENT, 0);

/* Output message, change title and icon */

	title (thread_num, "Working");
	OutputStr (thread_num, "Benchmarks complete, restarting worker.\n");
	ChangeIcon (thread_num, WORKING_ICON);
}

/**************************************************************/
/*           Routines dealing with priority work              */
/**************************************************************/

void start_priority_work_timer (void)
{
	if (SEQUENTIAL_WORK) return;
	add_timed_event (TE_PRIORITY_WORK, TE_PRIORITY_WORK_FREQ);
}

void stop_priority_work_timer (void)
{
	delete_timed_event (TE_PRIORITY_WORK);
}

/* Returns true if this is a priority work item */

int isPriorityWork (
	struct work_unit *w)
{
	if (w->work_type == WORK_ADVANCEDTEST) return (TRUE);
	if ((w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK) &&
	    (w->sieve_depth < w->factor_to || !w->pminus1ed))
		return (TRUE);
	if (w->work_type == WORK_PRP && w->tests_saved > 0.0)
		return (TRUE);
	return (FALSE);
}

/* For all threads, check if any of the Lucas-Lehmer test lines also */
/* require factoring. This will force factoring to be done first - giving */
/* us more accurate estimates of how much work is queued up. */

void check_for_priority_work (void)
{
	int	tnum;
	struct work_unit *w;

	for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++) {
		w = NULL;
		for ( ; ; ) {
			w = getNextWorkToDoLine (tnum, w, SHORT_TERM_USE);
			if (w == NULL) break;
			if (isPriorityWork (w)) {
				if (isWorkUnitActive (w)) {
					decrementWorkUnitUseCount (w, SHORT_TERM_USE);
					break;
				}
				stop_worker_for_priority_work (tnum);
			}
		}
       }
}

/**************************************************************/
/*     Routines dealing with stopping specific workers        */
/**************************************************************/

void mark_workers_active (
	int	thread_num)	/* Number of workers to mark active */
				/* or (<= 0) the only worker to mark */
{
	int	i;

	memset (ACTIVE_WORKERS, 0, sizeof (ACTIVE_WORKERS));
	for (i = 0; i < thread_num; i++) ACTIVE_WORKERS[i] = 1;
	if (thread_num <= 0) ACTIVE_WORKERS[-thread_num] = 1;
}

void start_one_worker (
	int	thread_num)
{
	if (thread_num < 0 || thread_num > (int) NUM_WORKER_THREADS) {
		OutputStr (MAIN_THREAD_NUM, "Worker thread number out of range.\n");
		return;
	}
	if (ACTIVE_WORKERS[thread_num]) {
		OutputStr (MAIN_THREAD_NUM, "Worker thread is already running.\n");
		return;
	}
	ACTIVE_WORKERS[thread_num] = 1;

	// Restart the worker
	restart_one_waiting_worker (thread_num, RESTART_USER_START);
}

void stop_one_worker (
	int	thread_num)
{
	if (thread_num < 0 || thread_num > (int) NUM_WORKER_THREADS) {
		OutputStr (MAIN_THREAD_NUM, "Worker thread number out of range.\n");
		return;
	}
	if (!ACTIVE_WORKERS[thread_num]) {
		OutputStr (MAIN_THREAD_NUM, "Worker thread is already stopped.\n");
		return;
	}
	ACTIVE_WORKERS[thread_num] = 0;
}

void implement_stop_one_worker (
	int	thread_num)
{

/* If some race condition has caused the worker active flag */
/* to be set, then do not wait for an event. */

	if (ACTIVE_WORKERS[thread_num]) return;

/* Output a worker stopping message and change the icon */

//// bug - this message will be output even if worker never started
	OutputStr (thread_num, "Stopping worker at user request.\n");
	ChangeIcon (thread_num, IDLE_ICON);	/* Idle icon while stopped */

/* Set memory usage to zero */

	set_memory_usage (thread_num, 0, 0);

/* Initialize and then wait for the event */

	gwevent_init (&USER_START_OR_STOP[thread_num]);
	gwevent_reset (&USER_START_OR_STOP[thread_num]);
	USER_START_OR_STOP_INITIALIZED[thread_num] = 1;
	gwevent_wait (&USER_START_OR_STOP[thread_num], 0);
	USER_START_OR_STOP_INITIALIZED[thread_num] = 0;
	gwevent_destroy (&USER_START_OR_STOP[thread_num]);

/* Output a worker starting message and change the icon */

////bug - why output a restart message if we are only resuming to exit?
	OutputStr (thread_num, "Resuming worker at user request.\n");
	ChangeIcon (thread_num, WORKING_ICON);
}

unsigned int active_workers_count (void)
{
	unsigned int i, count;

	for (i = count = 0; i < WORKER_THREADS_ACTIVE; i++)
		if (ACTIVE_WORKERS[i]) count++;
	return (count);
}

/**************************************************************/
/*        Routines dealing with "pause while running"         */
/**************************************************************/

/* Internal routine to parse a PauseWhileRunning or LowMemWhileRunning entry */

void parse_pause_info (
       char	*buf,		/* Comma separated list of program names */
       int	thread_num,	/* Worker thread to pause */
       int	low_mem)	/* Flag for LowMemWhileRunning */
{
	struct pause_info *data;
	char	*p, *bracket, *comma;

	if (*buf == 0) return;

	for (p = buf; ; p = comma + 1) {
		comma = strchr (p, ',');
		if (comma != NULL) *comma = 0;

		data = (struct pause_info *) malloc (sizeof (struct pause_info));
		if (data == NULL) return;
		data->next = PAUSE_DATA;
		PAUSE_DATA = data;

		data->thread_num = thread_num;
		data->low_mem = low_mem;
		bracket = strchr (p, '[');
		if (bracket != NULL) {
			*bracket = 0;
			data->workers_affected = atoi (bracket+1);
		} else
			data->workers_affected = MAX_NUM_WORKER_THREADS;

		if (!low_mem && *p == '*')
			data->program_name = NULL;
		else {
			data->program_name = (char *) malloc (strlen (p) + 1);
			if (data->program_name == NULL) return;
			strupper (p);
			strcpy (data->program_name, p);
		}

		if (comma == NULL) break;
	}
}

/* Read the PauseWhileRunning and LowMemWhileRunning settings */

void read_pause_info (void)
{
	int	tnum;
	char	buf[250];
	unsigned int seconds, seconds_until_reread;

/* Initalize the mutex */

	if (!PAUSE_MUTEX_INITIALIZED) {
		PAUSE_MUTEX_INITIALIZED = 1;
		gwmutex_init (&PAUSE_MUTEX);
	}

/* Lock just in case implement_pause is accessing this data */

	gwmutex_lock (&PAUSE_MUTEX);

/* Kill the timer that triggers rereading the pause info */

	delete_timed_event (TE_READ_PAUSE_DATA);

/* Free the previous pause data */

	while (PAUSE_DATA != NULL) {
		struct pause_info *p;
		p = PAUSE_DATA;
		PAUSE_DATA = p->next;
		if (p->program_name != NULL) free (p->program_name);
		free (p);
	}

/* Read and parse the PauseWhileRunning data from the ini file */

	seconds_until_reread = 0;
	IniGetTimedString (INI_FILE, "PauseWhileRunning", buf, sizeof (buf), NULL, &seconds);
	if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
		seconds_until_reread = seconds;
	parse_pause_info (buf, ALL_WORKERS, FALSE);
	for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++) {
		char	section_name[32];
		sprintf (section_name, "Worker #%d", tnum+1);
		IniSectionGetTimedString (INI_FILE, section_name, "PauseWhileRunning", buf, sizeof (buf), NULL, &seconds);
		if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
			seconds_until_reread = seconds;
		parse_pause_info (buf, tnum, FALSE);
	}
	PAUSE_WHILE_RUNNING_FREQ = IniGetTimedInt (INI_FILE, "PauseCheckInterval", 10, &seconds);
	if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
		seconds_until_reread = seconds;

/* Also read in the LowMemWhileRunning program list */

	IniGetTimedString (INI_FILE, "LowMemWhileRunning", buf, sizeof (buf), NULL, &seconds);
	if (seconds && (seconds_until_reread == 0 || seconds < seconds_until_reread))
		seconds_until_reread = seconds;
	parse_pause_info (buf, ALL_WORKERS, TRUE);

/* Add the event that fires when the memory settings expire. */

	if (seconds_until_reread)
		add_timed_event (TE_READ_PAUSE_DATA, seconds_until_reread);

/* If the pause timer is active, then call checkPauseWhileRunning so that */
/* we can decide which workers need to be paused based on this new pause info. */

	if (is_timed_event_active (TE_PAUSE_WHILE)) {
		delete_timed_event (TE_PAUSE_WHILE);
		checkPauseWhileRunning ();
	}

/* Unlock */

	gwmutex_unlock (&PAUSE_MUTEX);
}

void start_pause_while_running_timer (void)
{
	if (PAUSE_DATA == NULL) return;
	add_timed_event (TE_PAUSE_WHILE, 0);		// Check for pause-while-running programs immediately
}

void stop_pause_while_running_timer (void)
{
	delete_timed_event (TE_PAUSE_WHILE);
}

/* Internal routine to pick the "best" worker to pause */

int best_pause_candidate (
	struct pause_info **workers_to_pause)
{
	int	i;

/* Loop through all the workers.  Give preference to any worker that */
/* is paused waiting for work or is already paused. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (!ACTIVE_WORKERS[i]) continue;
		if (workers_to_pause[i] != NULL) continue;
		if (WORK_AVAILABLE_OR_STOP_INITIALIZED[i]) return (i);
		if (STOP_FOR_PAUSE[i] != NULL) return (i);
	}

/* Loop through all the workers.  Give preference to any worker that */
/* hasn't gotten started yet or is in low mem state but would rather */
/* be doing high mem work.  Note the MEM_RESTART_MAX_MEM_CHANGE flag */
/* is not checked because that is the flag that recomputes pfactor */
/* bounds on change in max mem. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (!ACTIVE_WORKERS[i]) continue;
		if (workers_to_pause[i] != NULL) continue;
		if (MEM_FLAGS[i] & MEM_USAGE_NOT_SET) return (i);
		if (MEM_FLAGS[i] & MEM_RESTARTING) return (i);
		if (MEM_RESTART_FLAGS[i] & ~MEM_RESTART_MAX_MEM_CHANGE) return (i);
	}

/* Loop through all the workers.  Return first one we find. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		if (!ACTIVE_WORKERS[i]) continue;
		if (workers_to_pause[i] != NULL) continue;
		return (i);
	}

/* Return 0 if we've paused all the workers */

	return (0);
}

/* Every time the pause-while-running timer fires, this routine is called */

void checkPauseWhileRunning (void)
{
	struct pause_info *p, *lowmem;
	struct pause_info *workers_to_pause[MAX_NUM_WORKER_THREADS];
	int	i, named_program_entries;

/* Clear flag indicating a running program matched a pause_info entry */

	named_program_entries = FALSE;
	for (p = PAUSE_DATA; p != NULL; p = p->next) {
		p->matching_program[0] = 0;
		if (p->program_name != NULL) named_program_entries = TRUE;
	}

/* Call OS-specific routine to see if a process is running such that */
/* we should pause.  This OS-specific routine must get the list of active */
/* processes and call isInPauseList for each one. */

	checkPauseListCallback ();

/* Examine pause info entries to see if a period of forced low memory usage */
/* should be in effect. */

	lowmem = NULL;
	for (p = PAUSE_DATA; p != NULL; p = p->next) {
		if (!p->low_mem) continue;
		if (p->matching_program[0]) lowmem = p;
	}
	p = STOP_FOR_LOW_MEMORY;
	STOP_FOR_LOW_MEMORY = lowmem;
	if (p == NULL && STOP_FOR_LOW_MEMORY != NULL) {
		char	buf[150];
		sprintf (buf, "Entering a period of low memory usage because %s is running.\n",
			 lowmem->matching_program);
		OutputStr (MAIN_THREAD_NUM, buf);
		stop_high_memory_workers ();
	}
	if (p != NULL && STOP_FOR_LOW_MEMORY == NULL) {
		restart_high_memory_workers ();
	}

/* Examine pause info entries to see which ones matched.  In this pass */
/* we are looking for pause_info entries that pause a specific worker. */

	memset (workers_to_pause, 0, sizeof (workers_to_pause));
	for (p = PAUSE_DATA; p != NULL; p = p->next) {
		if (p->thread_num == ALL_WORKERS) continue;
		if (p->low_mem) continue;
		if (p->program_name == NULL || p->matching_program[0])
			workers_to_pause[p->thread_num] = p;
	}

/* Examine pause info entries to see which ones matched.  In this pass */
/* we are looking for pause_info entries that let us choose which worker */
/* we want to pause.  Choose the "best" worker to pause. */

	for (p = PAUSE_DATA; p != NULL; p = p->next) {
		if (p->low_mem) continue;
		if (p->program_name == NULL || p->matching_program[0]) {
			int	count;
			count = p->workers_affected;
			if (p->thread_num != ALL_WORKERS) count--;
			for (i = 0; i < count; i++)
				workers_to_pause[best_pause_candidate (workers_to_pause)] = p;
		}
	}

/* We have now determined which workers we want to pause.  Compare that */
/* to the workers that are currently paused.  Pause more workers or */
/* resume workers as appropriate. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
		p = STOP_FOR_PAUSE[i];
		STOP_FOR_PAUSE[i] = workers_to_pause[i];
		if (p != NULL && STOP_FOR_PAUSE[i] == NULL)
			restart_one_waiting_worker (i, RESTART_END_PAUSE);
	}

/* If there are any pause-for-specific program entries, then we must reset */
/* the timer to check the pause list in a few seconds.  If there are only */
/* star (match any program) entries, then we don't need to check the pause */
/* list until new PauseWhileRunning info is read from the INI file.  However, */
/* do not delete the timer as read_pause_info checks for the timer being active! */	

	add_timed_event (TE_PAUSE_WHILE, named_program_entries ? PAUSE_WHILE_RUNNING_FREQ : 1000000);
}

/* This routine is called by the OS-specific routine that gets the process */
/* list.  It returns TRUE if an active process is in the pause-while-running */
/* list. */

void isInPauseList (
	char	*program_name)
{
	struct pause_info *p;
	char	buf[512];

	strcpy (buf, program_name);
	strupper (buf);
	for (p = PAUSE_DATA; p != NULL; p = p->next) {
		if (p->program_name != NULL &&
		    strstr (buf, p->program_name) != NULL) {
			buf[sizeof(p->matching_program)-1] = 0;
			strcpy (p->matching_program, buf);
		}
	}
}

/* This routine implements a pause for one worker thread */

void implement_pause (
	int	thread_num)
{
	struct pause_info *p;
	char	buf[140];

/* Lock so that read_pause_info cannot free our structure while we are accessing it */

	gwmutex_lock (&PAUSE_MUTEX);

/* Get the pause_info struct that is causing us to pause. */
/* Return quickly if the pause has already been cancelled. */	

	p = STOP_FOR_PAUSE[thread_num];
	if (p == NULL) {
		gwmutex_unlock (&PAUSE_MUTEX);
		return;
	}

/* Output an informative message.  If we are in a sleep time period */
/* (a "*" PauseWhileRunning entry) then output a different message than */
/* if we are pausing because a specific program is running. */

	if (p->program_name == NULL) {
		time_t	sleep_time;
		char	*time_as_string;

		sleep_time = timed_event_fire_time (TE_READ_PAUSE_DATA);
		time_as_string = sleep_time ? ctime (&sleep_time) : "forever";
		if (NUM_WORKER_THREADS == 1)
			sprintf (buf, "Sleeping until %s\n", time_as_string);
		else if (p->workers_affected == 1)
			sprintf (buf, "Sleeping one worker until %s\n", time_as_string);
		else if (p->workers_affected == MAX_NUM_WORKER_THREADS)
			sprintf (buf, "Sleeping all workers until %s\n", time_as_string);
		else
			sprintf (buf, "Sleeping %d workers until %s\n", p->workers_affected, time_as_string);
		OutputStr (thread_num, buf);
		title (thread_num, "Sleeping");
	} else {
		sprintf (buf, "Pausing because %s is running.\n", p->matching_program);
		OutputStr (thread_num, buf);
		title (thread_num, "Paused");
	}

/* Unlock */

	gwmutex_unlock (&PAUSE_MUTEX);

/* Wait for the end of the pause */

	gwevent_init (&END_PAUSE_OR_STOP[thread_num]);
	gwevent_reset (&END_PAUSE_OR_STOP[thread_num]);
	END_PAUSE_OR_STOP_INITIALIZED[thread_num] = 1;
	gwevent_wait (&END_PAUSE_OR_STOP[thread_num], 0);
	END_PAUSE_OR_STOP_INITIALIZED[thread_num] = 0;
	gwevent_destroy (&END_PAUSE_OR_STOP[thread_num]);

/* Output another informative message */

	OutputStr (thread_num, "Resuming processing.\n");
	title (thread_num, "Resuming");
}

/* This routine checks for a forced low memory situation */

int is_LowMemWhileRunning_active (
	int	thread_num)
{
	struct pause_info *p;
	char	buf[140];

/* Lock so that read_pause_info cannot free our structure while we are accessing it */

	gwmutex_lock (&PAUSE_MUTEX);

/* Get the pause_info struct that is causing the low memory situation. */
/* Return quickly if not in a low memory situation. */

	p = STOP_FOR_LOW_MEMORY;
	if (p == NULL) {
		gwmutex_unlock (&PAUSE_MUTEX);
		return (FALSE);
	}

/* Output an informative message.  If we are in a sleep time period */
/* (a "*" PauseWhileRunning entry) then output a different message than */
/* if we are pausing because a specific program is running. */

	sprintf (buf, "Cannot use lots of memory because %s is running.\n", p->matching_program);
	OutputStr (thread_num, buf);

/* Unlock */

	gwmutex_unlock (&PAUSE_MUTEX);
	return (TRUE);
}

/**************************************************************/
/*            Routines dealing with load average              */
/**************************************************************/

long LOAD_CHECK_TIME = 0;
double HI_LOAD = 0.0;
double LO_LOAD = 0.0;

void read_load_average_info (void)
{
	HI_LOAD = IniGetFloat (INI_FILE, "MaxLoad", 0.0);
	LO_LOAD = IniGetFloat (INI_FILE, "MinLoad", 0.0);
	LOAD_CHECK_TIME = IniGetInt (INI_FILE, "PauseTime", 20);
}

void start_load_average_timer (void)
{
	if (HI_LOAD <= 0.0 || LOAD_CHECK_TIME <= 0) return;
	if (get_load_average () < 0.0) return;
	add_timed_event (TE_LOAD_AVERAGE, LOAD_CHECK_TIME);
}

void stop_load_average_timer (void)
{
	delete_timed_event (TE_LOAD_AVERAGE);
}

/* Every time the pause-while-running timer fires, this routine is called */

void checkLoadAverage (void)
{
	double	load;
	long	recheck_interval;
	int	i;

/* Get the load average */

	load = get_load_average ();
	recheck_interval = LOAD_CHECK_TIME;

/* Check if we need to stop one or more workers. */
/* Wait at least a minute before rechecking the load */
/* This gives the system time to adjust the average to */
/* reflect our stopped worker. */

	if (load >= HI_LOAD) {
		double	threads_per_worker;
		int	workers_to_stop;

		threads_per_worker = (double) NUM_CPUS / (double) NUM_WORKER_THREADS;
		if (threads_per_worker < 1.0) threads_per_worker = 1.0;
		workers_to_stop = (int) ((load - HI_LOAD) / threads_per_worker);
		if (workers_to_stop < 1) workers_to_stop = 1;
		STOP_FOR_LOADAVG = workers_to_stop;
		if (recheck_interval < 65) recheck_interval = 65;
	}

/* Check if we need to restart a worker.  We restart workers */
/* one at a time so that we slowly build the load back up. */	
/* Wait at least a minute before rechecking the load to give the */
/* system time to adjust the average to reflect our restarted worker. */

	if (load >= 0.0 && load <= LO_LOAD) {
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) {
			if (END_LOADAVG_OR_STOP_INITIALIZED[i]) {
				restart_one_waiting_worker (i, RESTART_LOADAVG);
				if (recheck_interval < 65) recheck_interval = 65;
				break;
			}
		}
	}

/* Set the timer to check the load average again in the near future */

	add_timed_event (TE_LOAD_AVERAGE, recheck_interval);
}

/* This routine implements a load average pause for one worker thread */

void implement_loadavg (
	int	thread_num)
{
	char	buf[140];

/* Output an informative message. */

	sprintf (buf, "Pausing due to high load.\n");
	OutputStr (thread_num, buf);
	title (thread_num, "Paused");

/* Wait for the end of the high load */

	gwevent_init (&END_LOADAVG_OR_STOP[thread_num]);
	gwevent_reset (&END_LOADAVG_OR_STOP[thread_num]);
	END_LOADAVG_OR_STOP_INITIALIZED[thread_num] = 1;
	gwevent_wait (&END_LOADAVG_OR_STOP[thread_num], 0);
	END_LOADAVG_OR_STOP_INITIALIZED[thread_num] = 0;
	gwevent_destroy (&END_LOADAVG_OR_STOP[thread_num]);

/* Output another informative message */

	OutputStr (thread_num, "Resuming processing.\n");
	title (thread_num, "Resuming");
}

/**************************************************************/
/*             Routines dealing with throttling               */
/**************************************************************/

int	THROTTLE_SLEEP_TIME_IN_SEC = 0;
int	THROTTLE_SLEEP_TIME_IN_MS = 0;

void start_throttle_timer (void)
{
	if (THROTTLE_PCT <= 0 || THROTTLE_PCT >= 100) return;
	THROTTLE_SLEEP_TIME_IN_SEC = (int)
		((double) TE_THROTTLE_FREQ * (100.0 / (double) THROTTLE_PCT - 1.0));
	THROTTLE_SLEEP_TIME_IN_MS = (int)
		((double) (TE_THROTTLE_FREQ + THROTTLE_SLEEP_TIME_IN_SEC) *
			  (100.0 - (double) THROTTLE_PCT) * 10.0);
	add_timed_event (TE_THROTTLE, TE_THROTTLE_FREQ);
}

void stop_throttle_timer (void)
{
	delete_timed_event (TE_THROTTLE);
}

/* Every time the throttle timer fires, this routine is called */

int handleThrottleTimerEvent (void)
{
	stop_workers_for_throttle ();

/* Assume most threads will pause very soon.  Set timer to fire again */
/* after the idle time plus TE_THROTTLE_FREQ time.  That way each thread */
/* will run approximately TE_THROTTLE_FREQ seconds and idle */
/* THROTTLE_SLEEP_TIME_IN_SEC seconds for a CPU usage of THROTTLE_PCT. */

	return (TE_THROTTLE_FREQ + THROTTLE_SLEEP_TIME_IN_SEC);
}

/* This routine implements a throttle for one worker thread */

void implementThrottle (
	int	thread_num)
{
	int	totaltime;

/* Every 0.1 seconds see if we should resume processing.  We check */
/* frequently so that we can be responsive to an ESC or terminate command. */

	for (totaltime = 0; totaltime < THROTTLE_SLEEP_TIME_IN_MS; totaltime += 100) {
		if (WORKER_THREADS_STOPPING) return;
		Sleep (100);
	}
}

/**************************************************************/
/*                     Utility Routines                       */
/**************************************************************/

/* Return true is exponent yields a known Mersenne prime */

int isKnownMersennePrime (
	unsigned long p)
{
	return (p == 2 || p == 3 || p == 5 || p == 7 || p == 13 || p == 17 ||
		p == 19 || p == 31 || p == 61 || p == 89 || p == 107 ||
		p == 127 || p == 521 || p == 607 || p == 1279 || p == 2203 ||
		p == 2281 || p == 3217 || p == 4253 || p == 4423 ||
		p == 9689 || p == 9941 || p == 11213 || p == 19937 ||
		p == 21701 || p == 23209 || p == 44497 || p == 86243 ||
		p == 110503 || p == 132049 || p == 216091 || p == 756839 ||
		p == 859433 || p == 1257787 || p == 1398269 || p == 2976221 ||
		p == 3021377 || p == 6972593 || p == 13466917 ||
		p == 20996011 || p == 24036583 || p == 25964951 ||
		p == 30402457 || p == 32582657 || p == 37156667 ||
		p == 42643801 || p == 43112609 || p == 57885161 || p == 74207281 || p == 77232917);
}

/* Make a string out of a 96-bit value (a found factor) */

void makestr (
	unsigned long hsw,
	unsigned long msw,
	unsigned long lsw,
	char	*buf)			/* An 80 character output buffer */
{
	int	i, j, k, carry;
	unsigned long x[3];
	char	pow[80];

	x[0] = hsw; x[1] = msw; x[2] = lsw;
	for (i = 0; i < 79; i++) pow[i] = '0', buf[i] = '0';
	pow[78] = '1';
	pow[79] = buf[79] = 0;

	for (i = 3; i--; ) {
		for (j = 0; j < 32; j++) {
			if (x[i] & 1) {
				carry = 0;
				for (k = 79; k--; ) {
					buf[k] = buf[k] - '0' +
						pow[k] - '0' + carry;
					carry = buf[k] / 10;
					buf[k] %= 10;
					buf[k] += '0';
				}
			}
			carry = 0;
			for (k = 79; k--; ) {
				pow[k] = (pow[k] - '0') * 2 + carry;
				carry = pow[k] / 10;
				pow[k] %= 10;
				pow[k] += '0';
			}
			x[i] >>= 1;
		}
	}
	while (buf[0] == '0') safe_strcpy (buf, buf+1);
}

/* Sleep five minutes before restarting */

int SleepFive (
	int	thread_num)
{
	int	i;

	OutputStr (thread_num, ERRMSG4);
	BlinkIcon (thread_num, 10);		/* Blink icon for 10 seconds */
	for (i = 0; i < 100; i++) {
		Sleep (100);
		if (WORKER_THREADS_STOPPING) return (STOP_ESCAPE);
	}
	ChangeIcon (thread_num, IDLE_ICON);	/* Idle icon while stopped */
	for (i = 0; i < 2900; i++) {
		Sleep (100);
		if (WORKER_THREADS_STOPPING) return (STOP_ESCAPE);
	}
	ChangeIcon (thread_num, WORKING_ICON);	/* Back to the working icon */
	return (0);
}

/* Generate the scaling factors for ITER_OUTPUT in the rare cases where the user */
/* has used some undoc.txt settings to change how often the title is output or to */
/* make the frequency roughly the same in all windows even if using different FFT sizes. */

void calc_output_frequencies (
	gwhandle *gwdata,		/* Handle to the gwnum code */
	double	*output_frequency,	/* Calculated adjustment to ITER_OUTPUT */
	double	*output_title_frequency)/* Calculated adjustment to ITER_OUTPUT for title */
{
	int	scaled_freq, title_freq;
	double	exp, temp;

	/* Check the flag that says scale ITER_OUTPUT so that messages */
	/* appear at roughly same rate for all FFT sizes (scale factor */
	/* should be 1.0 if testing M50000000). */
	scaled_freq = (int) IniGetInt (INI_FILE, "ScaleOutputFrequency", 0);
	if (!scaled_freq) {
		*output_frequency = 1.0;
	} else {
		*output_frequency = gwmap_to_timing (1.0, 2, 50000000, -1) /
				    gwmap_to_timing (gwdata->k, gwdata->b, gwdata->n, gwdata->c);
		if (gwget_num_threads (gwdata) > 1 && NUM_WORKER_THREADS < NUM_CPUS)
			*output_frequency /= 1.8 * (gwget_num_threads (gwdata) - 1);
		/* For prettier output (outputs likely to be a multiple of a power of 10), round the */
		/* output frequency to the nearest (10,15,20,25,30,40,...90) times a power of ten */
		exp = floor (_log10 (*output_frequency));
		temp = *output_frequency * pow (10.0, -exp);
		if (temp < 1.25) temp = 1.0;
		else if (temp <1.75) temp = 1.5;
		else if (temp < 2.25) temp = 2.0;
		else if (temp < 2.75) temp = 2.5;
		else temp = floor (temp + 0.5);
		*output_frequency = temp * pow (10.0, exp);
	}

	/* Calculate the title frequency as a fraction of the output frequency */
	title_freq = (int) IniGetInt (INI_FILE, "TitleOutputFrequency", 1);
	if (title_freq < 1) title_freq = 1;
	*output_title_frequency = *output_frequency / (double) title_freq;
}

/* Truncate a percentage to the requested number of digits. */
/* Truncating prevents 99.5% from showing up as 100% complete. */

double trunc_percent (
	double	percent)
{
	percent *= 100.0;
	if (percent > 100.0) percent = 100.0;
	percent -= 0.5 * pow (10.0, - (double) PRECISION);
	if (percent < 0.0) return (0.0);
	return (percent);
}

/* Format the ETA for output to the worker window */

void formatETA (
	double	howlong,		/* how long to complete (in seconds) */
	char	*buf)
{
	double days, hours, minutes, seconds;
	days = floor (howlong / 86400.0);  howlong -= days * 86400.0;
	hours = floor (howlong / 3600.0);  howlong -= hours * 3600.0;
	minutes = floor (howlong / 60.0);  howlong -= minutes * 60.0;
	seconds = floor (howlong);
	if (days >= 3.0)
		sprintf (buf, ", ETA: %dd %02d:%02d", (int) days, (int) hours, (int) minutes);
	else
		sprintf (buf, ", ETA: %02d:%02d:%02d", (int) (days * 24.0 + hours), (int) minutes, (int) seconds);
}

/****************************************************************************/
/*             Portable routines to launch worker threads                   */
/****************************************************************************/

/* Structure used in launching one worker thread. */

struct LaunchData {
	int	thread_num;		/* This thread number */
	unsigned int num_threads;	/* Num threads to run */
	unsigned long p;		/* Exponent to time */
	unsigned long iters;		/* Iterations to time */
	int	bench_type;		/* Type of benchmark */
	int	delay_amount;		/* Seconds to delay starting worker */
	int	stop_reason;		/* Returned stop reason */
};

/* Create windows for the worker threads.  Windows REALLY prefers this be */
/* done in the main thread.  Otherwise, deadlocks can occur. */

void create_worker_windows (
	int	num_threads)
{
	int	tnum;
	char	buf[80];

/* Make sure each worker thread has a window to output to */

	for (tnum = 0; tnum < num_threads; tnum++) {
		create_window (tnum);
		if (NUM_CPUS * CPU_HYPERTHREADS > 1)
			sprintf (buf, "Worker #%d", tnum+1);
		else
			strcpy (buf, "Worker");
		base_title (tnum, buf);
	}
}

/* Launch the worker threads to process work units */

int LaunchWorkerThreads (
	int	thread_num,		/* Specific worker to launch or */
					/* special value ALL_WORKERS */
	int	wait_flag)		/* TRUE if we wait for workers to */
					/* end before returning. */
{
	struct LaunchData *ld;
	gwthread thread_handle;

/* If workers are already active, then call routine that restarts */
/* individual workers. */

	if (WORKER_THREADS_ACTIVE && (LAUNCH_TYPE == LD_CONTINUE || LAUNCH_TYPE == LD_TORTURE)) {
		if (thread_num == ALL_WORKERS) {
			for (thread_num = 0; thread_num < (int) NUM_WORKER_THREADS; thread_num++)
				if (! ACTIVE_WORKERS[thread_num])
					start_one_worker (thread_num);
		} else
			start_one_worker (thread_num);
		return (0);
	}

/* Create the launcher data structure, create the windows, then launch */

	ld = (struct LaunchData *) malloc (sizeof (struct LaunchData));
	if (ld == NULL) return (OutOfMemory (MAIN_THREAD_NUM));
	ld->num_threads = NUM_WORKER_THREADS;
	LAUNCH_TYPE = LD_CONTINUE;
	create_worker_windows (NUM_WORKER_THREADS);
	mark_workers_active (thread_num == ALL_WORKERS ? NUM_WORKER_THREADS : -thread_num);
	if (wait_flag) {
		gwthread_create_waitable (&thread_handle, &Launcher, ld);
		gwthread_wait_for_exit (&thread_handle);
	} else
		gwthread_create (&thread_handle, &Launcher, ld);
	return (0);
}

/* Launch threads to do a torture test */

int LaunchTortureTest (
	unsigned long num_threads,	/* Number of torture tests to run */
	int	wait_flag)		/* TRUE if we wait for workers to */
					/* end before returning. */
{
	struct LaunchData *ld;
	gwthread thread_handle;

	ld = (struct LaunchData *) malloc (sizeof (struct LaunchData));
	if (ld == NULL) return (OutOfMemory (MAIN_THREAD_NUM));
	ld->num_threads = num_threads;
	LAUNCH_TYPE = LD_TORTURE;
	create_worker_windows (num_threads);
	mark_workers_active (num_threads);
	if (wait_flag) {
		gwthread_create_waitable (&thread_handle, &Launcher, ld);
		gwthread_wait_for_exit (&thread_handle);
	} else
		gwthread_create (&thread_handle, &Launcher, ld);
	return (0);
}

/* Launch a thread to do a benchmark */

int LaunchBench (
	int	bench_type)		/* 0 = Throughput, 1 = FFT timings, 2 = Trial factoring */
{
	struct LaunchData *ld;
	gwthread thread_handle;

	ld = (struct LaunchData *) malloc (sizeof (struct LaunchData));
	if (ld == NULL) return (OutOfMemory (MAIN_THREAD_NUM));
	ld->num_threads = 1;
	ld->bench_type = bench_type;
	LAUNCH_TYPE = LD_BENCH;
	create_worker_windows (1);
	mark_workers_active (1);
	gwthread_create (&thread_handle, &Launcher, ld);
	return (0);
}

/* Launch the worker thread(s) to process Advanced/Time */

int LaunchAdvancedTime (
	unsigned long p,		/* Exponent to time */
	unsigned long iters)		/* Iterations to time */
{
	struct LaunchData *ld;
	gwthread thread_handle;

	ld = (struct LaunchData *) malloc (sizeof (struct LaunchData));
	if (ld == NULL) return (OutOfMemory (MAIN_THREAD_NUM));
	if (p >= 9900 && p <= 9919) ld->num_threads = p - 9900 + 1;
	else if (p >= 9920 && p <= 9939) ld->num_threads = p - 9920 + 1;
	else ld->num_threads = 1;
	ld->p = p;
	ld->iters = iters;
	LAUNCH_TYPE = LD_TIME;
	create_worker_windows (ld->num_threads);
	mark_workers_active (ld->num_threads);
	gwthread_create (&thread_handle, &Launcher, ld);
	return (0);
}

/* Launch all worker threads */

void Launcher (void *arg)
{
	struct LaunchData *ld;
	unsigned int tnum;
	int	stop_reason;
	gwthread handles[MAX_NUM_WORKER_THREADS];
	struct LaunchData *ldwork[MAX_NUM_WORKER_THREADS];
	int	delay_amount, total_delay_amount;

/* This thread will create more worker threads if necessary and */
/* then become thread number 0. */

	ld = (struct LaunchData *) arg;

/* If worker threads are active then stop them all.  This can */
/* happen when we choose Torture Test, Benchmark, or Advanced/Time from */
/* the menus while the worker threads are running */

	if (WORKER_THREADS_ACTIVE) {
		stop_workers_for_escape ();
		while (WORKER_THREADS_STOPPING) Sleep (50);
	}

/* Set flags so that GUI knows worker threads are active */

	WORKER_THREADS_ACTIVE = ld->num_threads;
	WORKER_THREADS_STOPPING = FALSE;

/* Output a starting worker threads message */

	if (ld->num_threads > 1)
		OutputStr (MAIN_THREAD_NUM, "Starting workers.\n");
	else
		OutputStr (MAIN_THREAD_NUM, "Starting worker.\n");

/* Every time the user chooses Test/Continue, clear any timers that */
/* prevents communication for a period of time.  This allows the user */
/* to try something and if it doesn't work, ESC and choose Test/Continue */
/* to try some other system settings (without waiting an hour). */

	clear_comm_rate_limits ();

/* Clear array of active thread handles */

again:	clearThreadHandleArray ();

/* Reread prime.ini, local.ini, and worktodo.ini files just in case user */
/* hand edited it.  We don't officially support this, but we'll do it */
/* anyway.  Also, check for a .add file, which we do officially support. */
/* If the user edited the ini files changing the number of worker threads */
/* then handle that here.  We also jump here if the threads were restarted */
/* because the user changed the number of worker threads using dialog boxes. */
/* NOTE: If the user increases the number of threads, then he will not see */
/* worker windows until he does a stop and restart. */

	stop_reason = readIniFiles ();
	if (stop_reason) {
		OutputStr (MAIN_THREAD_NUM, "Error rereading INI files.\n");
		return;
	}
	if (LAUNCH_TYPE == LD_CONTINUE) ld->num_threads = NUM_WORKER_THREADS;

/* Initialize flags that cause the worker threads to stop at the */
/* appropriate time */

	init_stop_code ();

/* Init the code that keeps track of the memory used by each worker thread */

	init_mem_state ();

/* Run OS-specific code prior to launching the worker threads */

	PreLaunchCallback (LAUNCH_TYPE);

/* Change the icon */

	ChangeIcon (MAIN_THREAD_NUM, WORKING_ICON);

/* Start all appropriate timers */

	if (LAUNCH_TYPE == LD_CONTINUE) {

/* Start timer that tells us to write save files every so often */

		start_save_files_timer ();

/* Start timer that tells us to run Jacobi error checks every so often */

		start_Jacobi_timer ();

/* Start the timer that checks battery status */

		start_battery_timer ();

/* Start the timer that checks for priority work */

		start_priority_work_timer ();

/* Start the timer that checks the pause-while-running list */

		start_pause_while_running_timer ();

/* Start the timer that checks the load average */

		start_load_average_timer ();

/* Start the throttle timer */

		start_throttle_timer ();
	}

/* Launch more worker threads if needed */

	delay_amount = IniGetInt (INI_FILE, "StaggerStarts", 5);
	total_delay_amount = 0;
	for (tnum = 1; tnum < ld->num_threads; tnum++) {
		ldwork[tnum] = (struct LaunchData *) malloc (sizeof (struct LaunchData));
		if (ldwork[tnum] == NULL) {
			OutOfMemory (MAIN_THREAD_NUM);
			return;
		}
		memcpy (ldwork[tnum], ld, sizeof (struct LaunchData));
		ldwork[tnum]->thread_num = tnum;
		total_delay_amount += delay_amount;
		ldwork[tnum]->delay_amount = total_delay_amount;
		gwthread_create_waitable (&handles[tnum], &LauncherDispatch, ldwork[tnum]);
	}

/* This thread is a worker thread too.  Call dispatching routine. */

	ld->thread_num = 0;
	ld->delay_amount = 0;
	LauncherDispatch (ld);
	stop_reason = ld->stop_reason;

/* Wait for other threads to finish */
/* Combine the stop reason with the stop reason returned by other threads */

	for (tnum = 1; tnum < ld->num_threads; tnum++) {
		gwthread_wait_for_exit (&handles[tnum]);
		if (stop_reason == 0)
			stop_reason = ldwork[tnum]->stop_reason;
		else if (stop_reason == STOP_ESCAPE ||
			 ldwork[tnum]->stop_reason == STOP_ESCAPE)
			stop_reason = STOP_ESCAPE;
		free (ldwork[tnum]);
	}

/* Write the worktodo file in case the WELL_BEHAVED_WORK flag caused us */
/* to delay writing the file. */

	if (LAUNCH_TYPE == LD_CONTINUE) {
		writeWorkToDoFile (TRUE);

/* Clear timers we started earlier */

		stop_save_files_timer ();
		stop_Jacobi_timer ();
		stop_battery_timer ();
		stop_priority_work_timer ();
		stop_pause_while_running_timer ();
		stop_load_average_timer ();
		stop_throttle_timer ();
	}

/* Change the icon */

	ChangeIcon (MAIN_THREAD_NUM, IDLE_ICON);

/* Run OS-specific code after worker threads terminate */

	PostLaunchCallback (LAUNCH_TYPE);

/* Restart all worker threads if the stop reason tells us to.  Make sure */
/* we set num_threads in case the reason for the restart is a change to */
/* NUM_WORKER_THREADS. */

	if (stop_reason == STOP_RESTART) {
		OutputStr (MAIN_THREAD_NUM, "Restarting all worker threads using new settings.\n");
		goto again;
	}

/* Restart all worker threads if the stop reason tells us to reread the */
/* INI file.  Make sure we set num_threads in case the reason for the restart */
/* is a change to NUM_WORKER_THREADS. */

	if (stop_reason == STOP_REREAD_INI) {
		OutputStr (MAIN_THREAD_NUM, "Restarting all worker threads using new timed prime.txt settings.\n");
		goto again;
	}

/* Output informative message */

	if (LAUNCH_TYPE == LD_CONTINUE || LAUNCH_TYPE == LD_TORTURE)
		OutputStr (MAIN_THREAD_NUM, "Execution halted.\n");
	if (LAUNCH_TYPE == LD_CONTINUE)
		OutputStr (MAIN_THREAD_NUM, "Choose Test/Continue to restart.\n");

/* Clear flags so that GUI knows worker threads are not active */

	WORKER_THREADS_ACTIVE = 0;
	WORKER_THREADS_STOPPING = FALSE;

/* Free the ld structure and exit the first worker thread */

	free (ld);
}

/* Now that the worker thread has been created, call the correct routine */
/* to do some work. */

void LauncherDispatch (void *arg)
{
	struct LaunchData *ld;
	int	stop_reason;

	ld = (struct LaunchData *) arg;

/* Handle a start delay here */

	if (ld->delay_amount && LAUNCH_TYPE == LD_CONTINUE) {
		char	buf[50];
		int	totaltime;

		title (ld->thread_num, "Waiting to start");
		sprintf (buf, "Waiting %d seconds to stagger worker starts.\n", ld->delay_amount);
		OutputStr (ld->thread_num, buf);

		for (totaltime = 0; totaltime < ld->delay_amount * 1000; totaltime += 100) {
			if (WORKER_THREADS_STOPPING) break;
			Sleep (100);
		}
	}

/* Output startup message */

	title (ld->thread_num, "Starting");
	OutputStr (ld->thread_num, "Worker starting\n");
	ChangeIcon (ld->thread_num, WORKING_ICON);

/* Dispatch to the correct code */

	switch (LAUNCH_TYPE) {
	case LD_CONTINUE:
		stop_reason = primeContinue (ld->thread_num);
		break;
	case LD_TIME:
		stop_reason = primeTime (ld->thread_num, ld->p, ld->iters);
		break;
	case LD_BENCH:
		stop_reason = primeBench (ld->thread_num, ld->bench_type);
		break;
	case LD_TORTURE:
		stop_reason = tortureTest (ld->thread_num, ld->num_threads);
		break;
	}

/* Change the title bar and output a line to the window */

	title (ld->thread_num, "Not running");
	OutputStr (ld->thread_num, "Worker stopped.\n");
	ChangeIcon (ld->thread_num, IDLE_ICON);

/* Set the return code and exit this worker thread */

	ld->stop_reason = stop_reason;
}

/****************************************************************************/
/*                       Process the work units                             */
/****************************************************************************/

/* Continue factoring/testing Mersenne numbers */

int primeContinue (
	int	thread_num)
{
	struct PriorityInfo sp_info;
	struct work_unit *w;
	unsigned int pass;
	int	stop_reason;

/* Set the process/thread priority */

	sp_info.type = SET_PRIORITY_NORMAL_WORK;
	sp_info.worker_num = thread_num;
	sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosity", 1);
	sp_info.normal_work_hyperthreads = 1;
	sp_info.aux_thread_num = 0;
	SetPriority (&sp_info);

/* Loop until the ESC key is hit or the entire work-to-do INI file */
/* is processed and we are not connected to the server. */

	for ( ; ; ) {

/* Check for a stop code.  We do this here in case the work-to-do file */
/* is empty (this call will be our only chance to check for a stop code). */

	stop_reason = stopCheck (thread_num);
	if (stop_reason) goto check_stop_code;

/* Clear flags that says we need to restart this thread if memory settings */
/* change.  If a work_unit cannot be processed because of a lack of */
/* available memory, then we will set these flags. */

	clear_memory_restart_flags (thread_num);

/* Make three passes over the worktodo.ini file looking for the ideal */
/* piece of work to do.  In pass 1, we look for high-priority work.  This */
/* includes trial and P-1 factoring prior to an LL test.  If a factor is */
/* found, it can reduce the amount of work we have queued up, requiring */
/* us to ask the server for more.  We also do AdvancedTest= lines in */
/* pass 1.  In pass 2, we process the file in order (except for LL tests */
/* that are not yet ready because the P-1 factoring has not completed). */
/* In pass 3, as a last resort we start P-1 stage 2 even if they will share */
/* memory with another P-1 in stage 2 and we start LL tests where P-1 */
/* factoring is stalled because of low memory. */
/* Skip first pass on large well-behaved work files. */

	for (pass = (WELL_BEHAVED_WORK || SEQUENTIAL_WORK) ? 2 : 1;
	     pass <= 3;
	     pass++) {

/* Examine each line in the worktodo.ini file */

	    for (w = NULL; ; ) {

/* Reset sp_info structure in case a previous work_unit changed these settings.  */
/* This actually happened when a TF job set normal_work_hyperthreads, and a */
/* subsequent LL job inappropriately started using hyperthreading. */

		sp_info.type = SET_PRIORITY_NORMAL_WORK;
		sp_info.worker_num = thread_num;
		sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosity", 1);
		sp_info.normal_work_hyperthreads = 1;
		sp_info.aux_thread_num = 0;

/* Read the line from the work file, break when out of lines */
/* Skip comment lines from worktodo.ini */

		w = getNextWorkToDoLine (thread_num, w, LONG_TERM_USE);
		if (w == NULL) break;
		if (w->work_type == WORK_NONE) continue;

/* Clear flags indicating this work_unit is using a lot of memory */

		set_default_memory_usage (thread_num);

/* Handle a factoring assignment */

		if (w->work_type == WORK_FACTOR && pass == 2) {
			stop_reason = primeFactor (thread_num, &sp_info, w, 0);
		}

/* Do special P-1 factoring work. */

		if (w->work_type == WORK_PFACTOR && pass == 2) {
			stop_reason = pfactor (thread_num, &sp_info, w);
		}

/* Run the LL test */

		if (w->work_type == WORK_ADVANCEDTEST ||
		    w->work_type == WORK_TEST ||
		    w->work_type == WORK_DBLCHK) {
			stop_reason = prime (thread_num, &sp_info, w, pass);
		}

/* See if this is an ECM factoring line */

		if (w->work_type == WORK_ECM && pass == 2) {
			stop_reason = ecm (thread_num, &sp_info, w);
		}

/* See if this is an P-1 factoring line */

		if (w->work_type == WORK_PMINUS1 && pass == 2) {
			stop_reason = pminus1 (thread_num, &sp_info, w);
		}

/* Run a PRP test */

		if (w->work_type == WORK_PRP) {
			stop_reason = prp (thread_num, &sp_info, w, pass);
		}

/* Set us back to default memory usage */

		set_default_memory_usage (thread_num);

/* If the work unit completed remove it from the worktodo.ini file and */
/* move on to the next entry */

		if (stop_reason == STOP_WORK_UNIT_COMPLETE) {
			rolling_average_work_unit_complete (thread_num, w);
			stop_reason = deleteWorkToDoLine (thread_num, w, FALSE);
		}

/* If a work unit could not be processed because there isn't enough memory, */
/* then move on to the next worktodo entry while we wait for more memory. */

		if (stop_reason == STOP_NOT_ENOUGH_MEM) {
			OutputStr (thread_num, "Looking for work that uses less memory.\n");
			stop_reason = 0;
		}

/* If we are aborting this work unit (probably because it is being deleted) */
/* then print a message. */

		if (stop_reason == STOP_ABORT)
			OutputStr (thread_num, "Aborting processing of this work unit.\n");

/* If stop reason is set then unlock this work unit and go process the */
/* stop reason.  Otherwise, no work was done, move on to the next entry */
/* in the worktodo.ini file. */

		if (stop_reason) {
			decrementWorkUnitUseCount (w, LONG_TERM_USE);
			goto check_stop_code;
		}

/* Process next work unit in the current pass */

	    }

/* Make another pass over the worktodo.ini file */

	}

/* Check for all the possible stop codes we must handle here.  Those */
/* that terminate the worker thread are not handled here. */

check_stop_code:

/* If we aborted a work unit (probably because it is being deleted) */
/* then loop to find next work unit to process. */

	if (stop_reason == STOP_ABORT) continue;

/* If we need to do priority work then reprocess the entire worktodo.ini. */

	if (stop_reason == STOP_PRIORITY_WORK) continue;

/* If we need to restart with the new memory settings, do so. */

	if (stop_reason == STOP_MEM_CHANGED) continue;

/* If the user is specifically stopping this worker, then stop until */
/* the user restarts the worker. */

	if (stop_reason == STOP_WORKER) {
		implement_stop_one_worker (thread_num);
		continue;
	}

/* If the worker is pausing because another program is running */
/* then implement that now. */

	if (stop_reason == STOP_PAUSE) {
		implement_pause (thread_num);
		continue;
	}

/* If the worker is pausing because we are now on battery power, then */
/* implement that now.  Semi-hack:  On Mac OS X, call the post/pre launch */
/* callback so that we allow the OS to re-enable Intel's power saving SpeedStep */
/* We pass in a dummy launch_type in case we might use that in the future. */

	if (stop_reason == STOP_BATTERY) {
		if (thread_num == MAIN_THREAD_NUM) PostLaunchCallback (9999);
		implement_stop_battery (thread_num);
		if (thread_num == MAIN_THREAD_NUM) PreLaunchCallback (9999);
		continue;
	}

/* If the worker is pausing for an auto-benchmark, then implement that now. */

	if (stop_reason == STOP_AUTOBENCH) {
		implement_stop_autobench (thread_num);
		continue;
	}

/* The stop reason was not caught above.  It must be a fatal error or a */
/* stop code that causes the worker thread to terminate. */

	if (stop_reason) return (stop_reason);

/* Ugh, we made three passes over the worktodo file and couldn't find */
/* any work to do.  I think this can only happen if we are low on memory */
/* or the worktodo file is empty. */

//bug? - only do this if two attempts are made at executing work?  Because
// work might have been added to the front of the file???

/* Output a message saying this worker thread is waiting for work */

	title (thread_num, "Waiting for work");
	OutputStr (thread_num, "No work to do at the present time.  Waiting.\n");
	ChangeIcon (thread_num, IDLE_ICON);

/* Set memory usage to zero */

	set_memory_usage (thread_num, 0, 0);

/* Spool a message to check the work queue.  Since we have no work queued */
/* up, this should cause us to get some work from the server. */

	spoolMessage (MSG_CHECK_WORK_QUEUE, NULL);

/* Wait for a mem-changed event OR communication attempt (it might get work) */
/* OR user entering new work via the dialog boxes OR the discovery of a .add */
/* file OR wait for a thread stop event (like ESC or shutdown). */

	gwevent_init (&WORK_AVAILABLE_OR_STOP[thread_num]);
	gwevent_reset (&WORK_AVAILABLE_OR_STOP[thread_num]);
	WORK_AVAILABLE_OR_STOP_INITIALIZED[thread_num] = 1;
	gwevent_wait (&WORK_AVAILABLE_OR_STOP[thread_num], 3600);
	WORK_AVAILABLE_OR_STOP_INITIALIZED[thread_num] = 0;
	gwevent_destroy (&WORK_AVAILABLE_OR_STOP[thread_num]);
	OutputStr (thread_num, "Resuming.\n");
	ChangeIcon (thread_num, WORKING_ICON);

/* Loop scanning the work-to-do file.  Hopefully the event triggered */
/* because we now have work to do. */

	}
}

/*************************/
/* Common save file code */
/*************************/

/* Internal routine to atomicly test for a unique file name.  If it is */
/* unique it is added to the list of save file names in use. */

int testUniqueFileName (
	int	thread_num,
	char	*filename)
{
static	int	USED_FILENAMES_MUTEX_INITIALIZED = FALSE;
static	gwmutex	USED_FILENAMES_MUTEX;
static	char	USED_FILENAMES[MAX_NUM_WORKER_THREADS][32];
	int	i;

/* Initialize the lock and used file array */

	if (!USED_FILENAMES_MUTEX_INITIALIZED) {
		USED_FILENAMES_MUTEX_INITIALIZED = 1;
		gwmutex_init (&USED_FILENAMES_MUTEX);
		for (i = 0; i < MAX_NUM_WORKER_THREADS; i++) USED_FILENAMES[i][0] = 0;
	}

/* Scan array to see if the save file name is in use by another thread. */

	gwmutex_lock (&USED_FILENAMES_MUTEX);
	for (i = 0; i < MAX_NUM_WORKER_THREADS; i++) {
		if (i != thread_num &&
		    strcmp (filename, USED_FILENAMES[i]) == 0) {
			gwmutex_unlock (&USED_FILENAMES_MUTEX);
			return (FALSE);
		}
	}

/* File name not in use, add the name to the array. */

	strcpy (USED_FILENAMES[thread_num], filename);
	gwmutex_unlock (&USED_FILENAMES_MUTEX);
	return (TRUE);
}

/* Multiple workers can do ECM on the same number.  This causes problems */
/* because the two threads try to use the same save file.  We work around */
/* the problem here, by making sure each worker has a unique save file name. */

void uniquifySaveFile (
	int	thread_num,
	char	*filename)
{
	char	original_filename[32];
	int	i;

/* Remember the orignal save file name */

	strcpy (original_filename, filename);

/* Our first preference is to use an existing save file with an extension */
/* consisting of this thread number */

	sprintf (filename, "%s_%d", original_filename, thread_num+1);
	if (fileExists (filename) && testUniqueFileName (thread_num, filename)) return;

/* Our second preference is to use an existing save file without any extensions */

	strcpy (filename, original_filename);
	if (fileExists (filename) && testUniqueFileName (thread_num, filename)) return;

/* Our third preference is to use any existing save file */

	for (i = 0; i < MAX_NUM_WORKER_THREADS; i++) {
		sprintf (filename, "%s_%d", original_filename, i+1);
		if (fileExists (filename) && testUniqueFileName (thread_num, filename)) return;
	}

/* Our fourth preference is to use the save file name without any extensions */

	strcpy (filename, original_filename);
	if (testUniqueFileName (thread_num, filename)) return;

/* Our fifth preference is to use an extension consisting of this thread number */

	sprintf (filename, "%s_%d", original_filename, thread_num+1);
	if (testUniqueFileName (thread_num, filename)) return;

/* Our final preference is to use any thread number as an extension */

	for (i = 0; i < MAX_NUM_WORKER_THREADS; i++) {
		sprintf (filename, "%s_%d", original_filename, i+1);
		if (testUniqueFileName (thread_num, filename)) return;
	}
}

/* Data structure used in reading save files and their backups as well as */
/* renaming bad save files. */

typedef struct read_save_file_state {
	int	thread_num;
	int	read_attempt;
	int	a_save_file_existed;
	int	a_non_bad_save_file_existed;
	int	num_original_bad_files;
	int	num_save_files_renamed;
	char	base_filename[80];
	char	current_filename[80];
} readSaveFileState;

/* Prepare for reading save files */

void readSaveFileStateInit (
	readSaveFileState *state,
	int	thread_num,
	char	*filename)
{
	state->thread_num = thread_num;
	state->read_attempt = 0;
	state->a_save_file_existed = 0;
	state->a_non_bad_save_file_existed = 0;
	state->num_original_bad_files = -1;
	state->num_save_files_renamed = 0;
	strcpy (state->base_filename, filename);
}

/* Prepare for reading save files.  Return TRUE if the save file or one */
/* of its backups exists. */

int saveFileExists (
	readSaveFileState *state)
{
	int	maxbad;
	char	buf[256];

/* Loop looking for progressively higher numbered save files */

	for ( ; ; ) {
		state->read_attempt++;

/* If the simple save file name exists, use it */

		if (state->read_attempt == 1) {
			strcpy (state->current_filename, state->base_filename);
			if (fileExists (state->current_filename)) goto winner;
			continue;
		}

/* If the second save file exists, use it */

		if (state->read_attempt == 2) {
			sprintf (state->current_filename, "%s.bu", state->base_filename);
			if (fileExists (state->current_filename)) goto winner;
			continue;
		}

/* If the third and higher save file exists (.bu2, .bu3, .bu4, etc.), use it */

		if (state->read_attempt <= NUM_BACKUP_FILES + NUM_JACOBI_BACKUP_FILES) {
			sprintf (state->current_filename, "%s.bu%d", state->base_filename, state->read_attempt - 1);
			if (fileExists (state->current_filename)) goto winner;
			continue;
		}

/* If the save file created during writing and before renaming exists, use it */

		if (state->read_attempt == NUM_BACKUP_FILES + NUM_JACOBI_BACKUP_FILES + 1) {
			state->read_attempt = 49;	// Special value to look for bad save files next
			sprintf (state->current_filename, "%s.write", state->base_filename);
			if (fileExists (state->current_filename)) goto winner;
			continue;
		}

/* Now retry old bad save files in case they were good but the OS was in a funky state earlier. */
/* Retry these files in highest to lowest order (but don't try any files we just renamed) so */
/* that more recent bad files are tried first. */

		if (state->num_original_bad_files >= 0) maxbad = state->num_original_bad_files;
		else maxbad = IniGetInt (INI_FILE, "MaxBadSaveFiles", 10);
		if (state->read_attempt >= 50 && state->read_attempt < 50 + maxbad) {
			sprintf (state->current_filename, "%s.bad%d", state->base_filename, maxbad - (state->read_attempt - 50));
			if (fileExists (state->current_filename)) goto winner;
			continue;
		}

/* We've run out of save files to look for */

		break;
	}

/* No useable save file found */

	return (FALSE);

/* We found a useable backup file.  Return so caller can try it. */

winner:	if (state->read_attempt != 1) {
		sprintf (buf, ALTSAVE_MSG, state->current_filename);
		OutputBoth (state->thread_num, buf);
	}
	state->a_save_file_existed = 1;
	if (state->read_attempt < 50) state->a_non_bad_save_file_existed = 1;
	return (TRUE);
}

/* Handle a bad save file.  We used to simply delete it, but we found some */
/* cases where the OS got in a funky state and could not read valid save files. */
/* Rather than lose the work done thusfar, we now rename the bad save file for */
/* possible later use. */

void saveFileBad (
	readSaveFileState *state)
{
	char	buf[256];
	char	filename[256];
	int	i, maxbad;

/* Print an error message indicating failure to read the save file */
	
	sprintf (buf, READFILEERR, state->current_filename);
	OutputBoth (state->thread_num, buf);

/* Don't rename a bad save file */

	if (state->read_attempt >= 50) return;

/* If we aren't renaming save files, then just return */

	maxbad = IniGetInt (INI_FILE, "MaxBadSaveFiles", 10);
	if (maxbad == 0) return;

/* If we haven't figured out how many bad save files existed originally, do so now */

	if (state->num_original_bad_files < 0) {
		state->num_original_bad_files = 0;
		for (i = 1; i <= maxbad; i++) {
			sprintf (buf, "%s.bad%d", state->base_filename, i);
			if (! fileExists (buf)) break;
			state->num_original_bad_files++;
		}
	}

/* If we don't have room for this save file, delete the oldest save file and rename other save files */

	if (state->num_original_bad_files + state->num_save_files_renamed >= maxbad) {
		sprintf (filename, "%s.bad1", state->base_filename);
		_unlink (filename);
		for (i = 2; i <= maxbad; i++) {
			char	oldname[80];
			sprintf (filename, "%s.bad%d", state->base_filename, i-1);
			sprintf (oldname, "%s.bad%d", state->base_filename, i);
			rename (oldname, filename);
		}
		if (state->num_original_bad_files) state->num_original_bad_files--;
		else state->num_save_files_renamed--;
	}

/* Rename the current file to a bad file */

	sprintf (filename, "%s.bad%d", state->base_filename, state->num_original_bad_files + state->num_save_files_renamed + 1);
	sprintf (buf, "Renaming %s to %s\n", state->current_filename, filename);
	OutputBoth (state->thread_num, buf);
	rename (state->current_filename, filename);
	state->num_save_files_renamed++;
}

/* Data structure used in writing save files and their backups */

typedef struct write_save_file_state {
	char	base_filename[80];
	int	num_ordinary_save_files;
	int	num_special_save_files;		/* Example: Number of save files to keep that passed the Jacobi error check */
	int	special;			/* Which ordinary save file is also special (zero if none) */
} writeSaveFileState;

/* Prepare for writing save files */

void writeSaveFileStateInit (
	writeSaveFileState *state,
	char	*filename,
	int	num_special_save_files)
{
	strcpy (state->base_filename, filename);
	state->num_ordinary_save_files = NUM_BACKUP_FILES;
	state->num_special_save_files = num_special_save_files;
	state->special = 0;			/* Init with "no ordinary file is special" */
}

/* Open the save file for writing.  Either overwrite or generate a temporary */
/* file name to write to, where we will rename the file after the file is */
/* successully written. */

int openWriteSaveFile (
	writeSaveFileState *state)
{
	char	output_filename[32];
	int	fd;

/* If we are allowed to create multiple intermediate files, then use a .write extension */
/* The value 99, not accessible via the GUI, is a special value meaning overwrite the */
/* existing save file -- a very dangerous choice.  You might use this for a floppy or */
/* small USB stick installation where there is no room for two save files. */
/* NOTE: This behavior is different than v24 where when the user selected one save */
/* file, then he got the dangerous overwrite option. */

	if (state->num_ordinary_save_files == 99)
		strcpy (output_filename, state->base_filename);
	else
		sprintf (output_filename, "%s.write", state->base_filename);

/* Now save to the intermediate file */

	fd = _open (output_filename, _O_BINARY | _O_WRONLY | _O_TRUNC | _O_CREAT, CREATE_FILE_ACCESS);
	return (fd);
}

/* Close the save file we finished writing.  If necessary, delete old */
/* save file, and rename just written save file. */

void closeWriteSaveFile (
	writeSaveFileState *state,
	int	fd)
{
	char	dest_filename[32], src_filename[32];
	int	rename_count;

/* Flush data to disk and close the save file. */

	_commit (fd);
	_close (fd);

/* If no renaming is needed, we're done */

	if (state->num_ordinary_save_files == 99) return;

/* Decide how many save files need renaming */

	rename_count = (state->special == state->num_ordinary_save_files) ?
			       state->num_ordinary_save_files + state->num_special_save_files : state->num_ordinary_save_files;

/* Delete the last file in the rename chain */

	if (rename_count == 1) strcpy (dest_filename, state->base_filename);
	else if (rename_count == 2) sprintf (dest_filename, "%s.bu", state->base_filename);
	else sprintf (dest_filename, "%s.bu%d", state->base_filename, rename_count-1);
	_unlink (dest_filename);

/* Perform the proper number of renames */

	while (rename_count--) {
		if (rename_count == 0) sprintf (src_filename, "%s.write", state->base_filename);
		else if (rename_count == 1) strcpy (src_filename, state->base_filename);
		else if (rename_count == 2) sprintf (src_filename, "%s.bu", state->base_filename);
		else sprintf (src_filename, "%s.bu%d", state->base_filename, rename_count-1);
		rename (src_filename, dest_filename);
		strcpy (dest_filename, src_filename);
	}

/* If there was an ordinary save file that was special, it is one step further down the chain */
/* or has finally reached the special save files. */

	if (state->special > 0 && state->special < state->num_ordinary_save_files) state->special++;
	else state->special = 0;
}

/* Close and delete the save file we were writing.  This is done */
/* when an error occurs while writing the save file. */

void deleteWriteSaveFile (
	writeSaveFileState *state,
	int	fd)
{
	char	output_filename[32];

/* Close and delete the save file */

	_close (fd);
	if (state->num_ordinary_save_files == 99)
		strcpy (output_filename, state->base_filename);
	else
		sprintf (output_filename, "%s.write", state->base_filename);
	_unlink (output_filename);
}

/* Delete save files when work unit completes. */

void unlinkSaveFiles (
	writeSaveFileState *state)
{
	int	i, maxbad;
	char	unlink_filename[80];

	maxbad = IniGetInt (INI_FILE, "MaxBadSaveFiles", 10);
	for (i = 1; i <= maxbad; i++) {
		sprintf (unlink_filename, "%s.bad%d", state->base_filename, i);
		_unlink (unlink_filename);
	}
	if (state->num_ordinary_save_files != 99) {
		for (i = 1; i < state->num_ordinary_save_files + state->num_special_save_files; i++) {
			if (i == 1) sprintf (unlink_filename, "%s.bu", state->base_filename);
			else sprintf (unlink_filename, "%s.bu%d", state->base_filename, i);
			_unlink (unlink_filename);
		}
	}
	if (state->base_filename[0] == 'p') {
		sprintf (unlink_filename, "q%s", state->base_filename+1);
		_unlink (unlink_filename);
		sprintf (unlink_filename, "r%s", state->base_filename+1);
		_unlink (unlink_filename);
	}
	sprintf (unlink_filename, "%s.write", state->base_filename);
	_unlink (unlink_filename);
	_unlink (state->base_filename);
}

/************************/
/* Trial Factoring code */
/************************/

/* This is for the legacy 32-bit factoring code.  We no longer actively work on this code */

#ifndef X86_64

/* This defines the C / assembly language communication structure */

#define NEW_STACK_SIZE	(4096+256)
struct facasm_data {
	uint32_t EXPONENT;		/* Mersenne number to factor */
	uint32_t FACPASS;		/* Which of 16 factoring passes */
	uint32_t FACHSW;		/* High word of found factor */
	uint32_t FACMSW;		/* Middle word of found factor */
	uint32_t FACLSW;		/* Low word of found factor */
	uint32_t cpu_flags;		/* Copy of CPU_FLAGS */
	uint32_t firstcall;		/* Flag set on first facpasssetup */
	uint32_t pad[5];
	uint32_t xmm_data[188];		/* XMM data initialized in C code */
};

/* This defines the factoring data handled in C code.  The handle */
/* abstracts all the internal details from callers of the factoring code. */

typedef struct {
	int	num_threads;		/* Number of threads to use in factoring.  Not supported in 32-bit code. */
	double	endpt;			/* Factoring limit for this pass.  Not needed in 32-bit setup. */
	struct PriorityInfo *sp_info;	/* Priority structure for setting aux thread priority */
	struct	facasm_data *asm_data;	/* Memory for factoring code */
} fachandle;

EXTERNC void setupf (struct facasm_data *);	/* Assembly code, setup */
EXTERNC int factor64 (struct facasm_data *);	/* Assembly code, do work */

/* Prepare a factoring run */

int factorSetup (
	int	thread_num,
	unsigned long p,
	fachandle *facdata)
{
	void	*asm_data_alloc;
	struct facasm_data *asm_data;

/* Clear fachandle.  A presently unnecessary precaution to ensure factorDone won't try to free uninitialized pointers */

	memset (facdata, 0, sizeof (fachandle));

/* Allocate 1MB for the assembly code global data.  This area is preceded */
/* by a temporary stack.  This allows the assembly code to access the global */
/* data using offsets from the stack pointer.  We zero the first 64KB, */
/* asm code requires this (such as XMM_COMPARE_VALn). */

	asm_data_alloc = aligned_malloc (1000000, 4096);
	if (asm_data_alloc == NULL) {
		OutputStr (thread_num, "Error allocating memory for trial factoring.\n");
		return (STOP_OUT_OF_MEM);
	}
	facdata->asm_data = asm_data = (struct facasm_data *) ((char *) asm_data_alloc + NEW_STACK_SIZE);
	memset (asm_data, 0, 65536);

/* Init */

	asm_data->EXPONENT = p;
	asm_data->cpu_flags = CPU_FLAGS;
	asm_data->firstcall = 0;
	if (!IniGetInt (LOCALINI_FILE, "FactorUsingSSE2", 1)) asm_data->cpu_flags &= ~CPU_SSE2;

/* Setup complete */

	return (0);
}

/* Prepare for one of the 16 factoring passes */

int factorPassSetup (
	int	thread_num,
	unsigned long pass,
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	struct facasm_data *asm_data;

/* Call the factoring setup assembly code */

	asm_data = (struct facasm_data *) facdata->asm_data;
	asm_data->FACPASS = pass;
	setupf (asm_data);

/* If using the SSE2 factoring code, do more initialization */
/* We need to initialize much of the following data: */
/*	XMM_INITVAL		DD	0,0,0,0
	XMM_INVFAC		DD	0,0,0,0
	XMM_I1			DD	0,0,0,0
	XMM_I2			DD	0,0,0,0
	XMM_F1			DD	0,0,0,0
	XMM_F2			DD	0,0,0,0
	XMM_F3			DD	0,0,0,0
	XMM_TWO_120_MODF1	DD	0,0,0,0
	XMM_TWO_120_MODF2	DD	0,0,0,0
	XMM_TWO_120_MODF3	DD	0,0,0,0
	XMM_INIT120BS		DD	0,0
	XMM_INITBS		DD	0,0
	XMM_BS			DD	0,0
	XMM_SHIFTER		DD	64 DUP (0)
	TWO_TO_FACSIZE_PLUS_62	DQ	0.0
	SSE2_LOOP_COUNTER	DD	0 */

	if (asm_data->cpu_flags & CPU_SSE2) {
		unsigned long i, p, bits_in_factor;
		uint32_t *xmm_data;

/* Compute the number of bits in the factors we will be testing */

		if (asm_data->FACHSW)
			bits_in_factor = 64, i = asm_data->FACHSW;
		else if (asm_data->FACMSW)
			bits_in_factor = 32, i = asm_data->FACMSW;
		else return (0);
		while (i) bits_in_factor++, i >>= 1;

/* Factors 63 bits and below use the non-SSE2 code */

		if (bits_in_factor <= 63) return (0);

/* Set XMM_SHIFTER values (the first shifter value is not used). */
/* Also compute the initial value. */

		xmm_data = asm_data->xmm_data;
		p = asm_data->EXPONENT;
		for (i = 0; p > bits_in_factor + 59; i++) {
			xmm_data[48+i*2] = (p & 1) ? 1 : 0;
			p >>= 1;
		}
		xmm_data[0] =					/* XMM_INITVAL */
		xmm_data[2] = p >= 90 ? 0 : (1 << (p - 60));
		xmm_data[40] = 62 - (120 - bits_in_factor);	/* XMM_INIT120BS */
		xmm_data[42] = 62 - (p - bits_in_factor);	/* XMM_INITBS */
		xmm_data[44] = bits_in_factor - 61;		/* Set XMM_BS to 60 - (120 - fac_size + 1) as defined in factor64.mac */
		xmm_data[112] = i;				/* SSE2_LOOP_COUNTER */
		*(double *)(&xmm_data[110]) =			/* TWO_TO_FACSIZE_PLUS_62 */
			pow ((double) 2.0, (int) (bits_in_factor + 62));
	}

/* Setup complete */

	return (0);
}

/* Factor one "chunk".  The assembly code decides how big a chunk is. */

#define FACTOR_CHUNK_SIZE		16			// 32-bit sieve processes four 4KB sieves

int factorChunk (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	return (factor64 (facdata->asm_data));
}

/* Number of chunks that factorChunk processed.  This can be more than one when multithreading in 64-bit code */

int factorChunksProcessed (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	return (1);
}

/* Find sieved area with smallest first factor so that we can write a save file */

void factorFindSmallestNotTFed (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	return;
}

/* Cleanup after making a factoring run */

void factorDone (
	fachandle *facdata)		/* Handle returned by factorSetup */
{

/* Free assembly code work area */

	if (facdata->asm_data != NULL) {
		aligned_free ((char *) facdata->asm_data - NEW_STACK_SIZE);
		facdata->asm_data = NULL;
	}
}

/* Return a second or third found factor.  Can only happen in 64-bit code. */

int getAnotherFactorIfAny (
	fachandle *facdata)
{
	return (FALSE);
}

#endif


/* And this is the 64-bit factoring code.  This was split off from the 32-bit factoring */
/* code when multi-threading feature was added. */

#ifdef X86_64

#define SIEVE_SIZE_IN_BYTES	(12 * 1024)		// 12KB sieve in asm code
#define INITSIEVE_PRIMES1	(7 * 11 * 13 * 17)	// Initialize sieve with these 4 primes cleared (uses 29KB)
#define INITSIEVE_PRIMES2	(11 * 13 * 17 * 19)	// Initialize sieve with these 4 primes cleared (uses 57KB)
#define INITSIEVE_PRIMES3	(13 * 17 * 19 * 23)	// Initialize sieve with these 4 primes cleared (uses 108KB)
#define INITSIEVE_PRIMES4	(17 * 19 * 23)		// Initialize sieve with these 3 primes cleared (uses 19KB)

#define MAX_TF_FOUND_COUNT	10			// Maximum number of factors we'll find in a single pass

/* This defines the C / assembly language communication structure */
/* When multi-threading, each thread has its own copy of this structure */

struct facasm_data {
	uint64_t p;			/* Exponent of Mersenne number to factor */
	uint64_t facdists[65];		/* Multiples of 120 * p * num_siever_allocs */
	uint64_t facdist12K;		/* Factor distance covered by a 12K sieve */
	uint64_t savefac1;		/* LSW or first factor in sieve */
	uint64_t savefac0;		/* MSW or first factor in sieve */
	void	*sieve;			/* Area to sieve or already sieved area to TF */
	void	*primearray;		/* Array of sieving primes */
	void	*offsetarray;		/* Array of bits-to-clear for each sieving prime */
	void	*initsieve;		/* Array used to initialize sieve */
	double	TWO_TO_FACSIZE_PLUS_62;	/* Constant used in SSE2/AVX2/AVX512 integer TF code */

	uint32_t FACHSW;		/* High word of found factor */
	uint32_t FACMSW;		/* Middle word of found factor */
	uint32_t FACLSW;		/* Low word of found factor */
	uint32_t cpu_flags;		/* Copy of CPU_FLAGS */
	uint32_t SSE2_LOOP_COUNTER;	/* Counter used in SSE2/AVX2/FMA/AVX512 TF code */
	uint32_t FMA_INITVAL_TYPE;	/* Type of initval (more or less than 2^split).  Used in FMA code. */
	uint32_t FMA_64_MINUS_LOW_BITS;	/* 64 minus bits in a low word.  Used in FMA code. */
	uint32_t initstart;		/* First byte in initsieve to copy (see ASM code) */
	uint32_t alternate_sieve_count;	/* Number of 13-small-prime AVX2 sieve sections to process */

	uint32_t pad[3];		/* Pad to next 64-byte cache line */
	uint32_t xmm_data[80];		/* XMM/YMM/ZMM data initialized in C code */

	uint32_t other_asm_temps_and_consts[600];
};

/* This defines the factoring data handled in C code.  The handle */
/* abstracts all the internal details from callers of the factoring code. */
/* When multi-threading, there is only one copy of this structure.  Obviously, */
/* one must obtain a lock before accessing structure members. */

struct tf_thread_info {
	int	pool;				/* Pool this thread belongs to */
	int	thread_can_sieve;		/* Flag indicating this thread can do sieving. */
	int	last_siever_used;		/* Last siever this thread used.  Try to use it again for good locality. */
	int	last_sieve_area_used;		/* Last sieve area this thread used.  Try to use it again for good locality. */
	gwthread thread_id;			/* Auxiliary thread id */
};

struct siever_info {
	int	state;				/* State of this siever (see below) */
	int	last_thread_used;		/* Last thread to use this siever */
	uint64_t next_sieve_first_factor[2];	/* First factor for next sieve area */
	void	*offsetarray;			/* bit-to-clear offsets for each sieve prime */
	uint64_t num_areas_to_sieve;		/* Count of remaining needed factor64_sieve calls for this siever */
};

struct sieve_area_info {
	int	state;				/* State of this sieve area (see below) */
	uint64_t first_factor[2];		/* First factor of the sieved area */
	void	*sieve;				/* The 12KB sieved bit array */
};

struct pool_info {
	int	num_sievers_active;		/* Count of threads currently running the sieving assembly code */
	int	num_free_sieve_areas;		/* Count of sieve areas that are free */
	int	num_sieved_sieve_areas;		/* Count of sieve areas that are sieved */
	int	last_sieve_area_for_sieve;	/* When pooling, last sieve area handed out for sieving */
	int	last_sieve_area_for_tf;		/* When pooling, last sieve area handed out for TF */
	struct sieve_area_info *sieve_areas;	/* Ptr to info about each sieve area */
};

typedef struct {
	int	num_threads;			/* Number of threads to use in factoring. */
	struct tf_thread_info *tf_threads;	/* Ptr to info about each TF thread */

	int	num_pools;			/* Number of pools */
	int	num_threads_per_pool;		/* Number of threads assigned to each pool */
	int	num_sieve_areas_per_pool;	/* Number of 12KB sieve areas in each pool */
	int	pooling;			/* If not pooling, each thread immediately TFs the sieve area after being sieved */
	struct pool_info *pools;		/* Ptr to info about each pool */

	int	num_sievers;			/* Number of allocated sievers (residue classes of num_siever_groups) */
	struct siever_info *sievers;		/* Ptr to info about each allocated siever */
	uint64_t total_num_areas_to_sieve;	/* Count of remaining needed factor64_sieve calls */

	double	endpt;				/* Factoring limit for this pass */
	struct PriorityInfo *sp_info;		/* Priority structure for setting aux thread priority */
	struct	facasm_data *asm_data;		/* Main thread's memory for assembly factoring code */
	int	factoring_pass;			/* Which of the 16 factoring passes we are on */
	int	pass_complete;			/* Set when a factoring pass completes */
	uint32_t initsieve_primes;		/* Which primes are sieved when initializing from initsieve */
	uint32_t one_eighth_initsieve_primes;	/* (1 / (8 * facdist1)) mod initsieve_primes.  Used to calculate first byte to copy from initsieve */
	uint32_t *modinvarray;			/* Modular inverse of facdist for each sieving prime.  Used to set offsetarray. */
	int	num_chunks_TFed;		/* Count of chunks TFed since last call to factorChunksProcessed */
	uint64_t total_num_chunks_TFed;		/* Number of chunks TFed this pass */
	uint64_t total_num_chunks_to_TF;	/* Number of chunks to TF this pass */
	uint32_t found_lsw[MAX_TF_FOUND_COUNT];	/* LSW of a found factor */
	uint32_t found_msw[MAX_TF_FOUND_COUNT];	/* MSW of a found factor */
	uint32_t found_hsw[MAX_TF_FOUND_COUNT];	/* HSW of a found factor */
	uint32_t found_count;			/* Count of found factors */
	unsigned int num_active_threads;	/* Count of the number of active auxiliary threads */
	int	threads_must_exit;		/* Flag set to force all auxiliary threads to terminate */
	gwmutex	thread_lock;			/* This mutex limits one thread at a time in critical sections. */
	gwevent	thread_work_to_do;		/* This event is set whenever the auxiliary threads have work to do. */
	gwevent	sieved_areas_available;		/* This event is set whenever a sieved area is made available for TF. */
	gwevent	all_threads_started;		/* This event is set whenever the auxiliary threads are done and the */
	gwevent	all_threads_done;		/* This event is set when all the auxiliary threads have started */
						/* main thread can resume.  That is, it is set when num_active_threads==0 */
} fachandle;

/* Possible states for a siever */

#define SIEVER_ASSIGNED			0x8000	/* Set if the siever has been assigned to a thread */
#define SIEVER_ACTIVE			0	/* Thread actively sieving */
#define SIEVER_INACTIVE			1	/* Thread not actively sieving */
#define SIEVER_UNINITIALIZED		2	/* Thread not fully initialized */

/* Possible states for a sieve area */

#define SIEVE_AREA_FREE			0	/* Allocated & free */
#define SIEVE_AREA_SIEVING		1	/* Area currently being sieved */
#define SIEVE_AREA_SIEVED		2	/* Area sieved, ready for TF */
#define SIEVE_AREA_TFING		3	/* Area currently being TFed */

/* ASM entry points */

EXTERNC int factor64_pass_setup (struct facasm_data *);	/* Assembly code, setup required for each mod-120 pass */
EXTERNC int factor64_small (struct facasm_data *);	/* Assembly code, brute force TF for factors below 2^44 */
EXTERNC int factor64_sieve (struct facasm_data *);	/* Assembly code, sieve a block */
EXTERNC int factor64_tf (struct facasm_data *);		/* Assembly code, TF a sieved block */

/* Forward declarations */

int factorChunkMultithreaded (fachandle *facdata, struct facasm_data *asm_data, int aux_thread_num);
void factorFindSmallestNotTFed (fachandle *facdata);

/* Routine for auxiliary threads */

struct factor_thread_data {
	fachandle *facdata;				/* The global facdata */
	struct facasm_data *asm_data;			/* This thread's asm_data */
	int	thread_num;
};

void factor_auxiliary_thread (void *arg)
{
	struct factor_thread_data *info;
	fachandle *facdata;
	struct facasm_data *asm_data, *main_thread_asm_data;

/* Get pointers to various structures */

	info = (struct factor_thread_data *) arg;
	facdata = info->facdata;
	asm_data = info->asm_data;
	main_thread_asm_data = facdata->asm_data;

/* Set thread priority (0 = thread starting) */

	SetAuxThreadPriority (info->thread_num, 0, facdata->sp_info);

/* Loop waiting for work to do.  The main thread will signal the */
/* thread_work_to_do event whenever there is work for the auxiliary */
/* thread to do. */

	for ( ; ; ) {
		gwevent_wait (&facdata->thread_work_to_do, 0);
		if (facdata->threads_must_exit) break;

/* Each thread needs its own copy of the asm_data.  Copy the structure after each factor_pass_setup */

		memcpy (asm_data, main_thread_asm_data, sizeof (struct facasm_data));

/* Increment count of active auxiliary threads.  Once all auxiliary threads have started reset the */
/* thread_work_to_do event and signal the all_threads_started event */

		gwmutex_lock (&facdata->thread_lock);
		facdata->num_active_threads++;
		if (facdata->num_active_threads == facdata->num_threads - 1) {
			gwevent_reset (&facdata->thread_work_to_do);
			gwevent_signal (&facdata->all_threads_started);
		}
		gwmutex_unlock (&facdata->thread_lock);

/* Loop factoring chunks */

		for ( ; ; ) {
			if (facdata->threads_must_exit) break;
			if (factorChunkMultithreaded (facdata, asm_data, info->thread_num)) break;
		}

/* Wait for all auxiliary threads to start before we start decrementing the count of active threads */

		gwevent_wait (&facdata->all_threads_started, 0);

/* The auxiliary thread has run out of work.  Decrement the count of active auxiliary threads. */
/* Signal all threads done when last auxiliary thread is done. */

		gwmutex_lock (&facdata->thread_lock);
		facdata->num_active_threads--;
		if (facdata->num_active_threads == 0) gwevent_signal (&facdata->all_threads_done);
		gwmutex_unlock (&facdata->thread_lock);
		if (facdata->threads_must_exit) break;
	}

/* Set thread priority (1 = thread terminating) */

	SetAuxThreadPriority (info->thread_num, 1, NULL);

/* Free the allocated memory and exit the auxiliary thread */

	aligned_free (info->asm_data);
	free (arg);
}


/* Prepare for a factoring run */

int factorSetup (
	int	thread_num,
	unsigned long p,
	fachandle *facdata)
{
	struct facasm_data *asm_data;
	int	i, pct, mem_needed;
	struct PriorityInfo *sp_info;
	int	num_small_primes;		/* Number of primes the ASM code will use for sieving */
	int	num_siever_groups;		/* Multipler for facdist, num_sievers = number of residue classes of num_siever_groups */
	int	num_siever_threads;		/* Total number of threads that can be sieving */
	int	num_siever_threads_per_pool;	/* Number of threads in each pool that can sieve */
			
/* Clear fachandle.  A precaution to ensure factorDone won't try to free uninitialized pointers. */

	i = facdata->num_threads;
	sp_info = facdata->sp_info;
	memset (facdata, 0, sizeof (fachandle));
	facdata->num_threads = i;
	facdata->sp_info = sp_info;

/* Allocate memory for the assembly code global data.  We zero the struct, asm code requires this. */

	facdata->asm_data = asm_data = (struct facasm_data *) aligned_malloc (sizeof (struct facasm_data), 64);
	if (asm_data == NULL) {
memerr:		OutputStr (thread_num, "Error allocating memory for trial factoring.\n");
		return (STOP_OUT_OF_MEM);
	}
	memset (asm_data, 0, sizeof (struct facasm_data));

/* Init asm_data.  Note the default is to not use SSE2 TF code in 64-bit mode (unlike 32-bit prime95). */
/* This decision was made years ago when, IIRC, Opteron's were faster using the non-SSE2 code */
/* and the Intel CPUs were ambivalent. */

	asm_data->p = p;
	asm_data->cpu_flags = CPU_FLAGS;
	if (!IniGetInt (LOCALINI_FILE, "FactorUsingSSE2", 0)) asm_data->cpu_flags &= ~CPU_SSE2;
	asm_data->alternate_sieve_count = IniGetInt (INI_FILE, "AlternateTFSieveCount", 9);
	if (asm_data->alternate_sieve_count < 1) asm_data->alternate_sieve_count = 1;

/* Get the number of threads in each pool.  We use pools of threads and sieve areas to improve locality. */
/* Threads in a pool are guaranteed to TF a 12KB sieve area that was sieved by one of the threads in the same pool. */
/* If the threads in a pool share an L1, L2, or L3 cache then locality is improved.  Our default is to put hyperthreads */
/* in the same pool since they share the same L1 cache.  On Knight's Landing we might consider putting each tile which */
/* has 2 cores of 4 hyperthreads that share an L2 cache in the same pool. */

/* NOTE:  Pools are only useful if PercentTFSieverThreads is set to less than the default 100. */
/* Otherwise, each thread sieves and then immediately TFs -- perfect locality.  We've kept the pooling code */
/* because it might be beneficial on a hyperthreaded machine.  If one hyperthread is always TFing and the */
/* other hyperthread is either sieving or TFing, then throughput MAY be improved because the AVX units are */
/* always in use doing TF.  In other words, two concurrent sievers may not hyperthread as well as a siever and a TFer. */

	facdata->num_threads_per_pool = IniGetInt (INI_FILE, "ThreadsPerTFPool", CPU_HYPERTHREADS);
	if (facdata->num_threads_per_pool < 1) facdata->num_threads_per_pool = 1;
	if (facdata->num_threads_per_pool > facdata->num_threads) facdata->num_threads_per_pool = facdata->num_threads;
	if (facdata->num_threads % facdata->num_threads_per_pool) facdata->num_threads_per_pool = 1;

/* Allocate a structure for each TF thread */

	mem_needed = facdata->num_threads * sizeof (struct tf_thread_info);
	facdata->tf_threads = (struct tf_thread_info *) malloc (mem_needed);
	if (facdata->tf_threads == NULL) goto memerr;
	memset (facdata->tf_threads, 0, mem_needed);

/* Allocate structures for each thread pool */

	facdata->num_pools = facdata->num_threads / facdata->num_threads_per_pool;
	mem_needed = facdata->num_pools * sizeof (struct pool_info);
	facdata->pools = (struct pool_info *) malloc (mem_needed);
	if (facdata->pools == NULL) goto memerr;
	memset (facdata->pools, 0, mem_needed);

/* Initialize maximum number of siever threads in each pool (based on a percent specified in the INI file) */
/* For many older architectures one siever can easily keep four TF threads busy. */
/* For FMA architectures (our fastest TF code), one siever does not keep 4 TF threads busy. */
/* Knights Landing with AVX512 is another exception, needing at least 50% sievers. */
/* NOTE:  The pooling code that operates sieving indepenently from TFing was written first. */
/* Subsequently, I tried sieving and immediately TFing to improve locality (not pooling). */
/* Despite the perfect locality, timings were a little worse on both SkyLake and Knight's Landing. */
/* NOTE: Pooling is turned off by setting PercentTFSieverThreads to 100. */

	pct = IniGetInt (INI_FILE, "PercentTFSieverThreads", 50);
	if (pct < 1) pct = 1;
	if (pct > 100) pct = 100;
	facdata->pooling = (pct != 100);
	num_siever_threads_per_pool = (pct * facdata->num_threads_per_pool + 99) / 100;

/* Initialize threads info (what pool the thread is in and if the thread can sieve) */

	for (i = 0; i < facdata->num_threads; i++) {
		int	thread_within_pool;
		facdata->tf_threads[i].pool = i / facdata->num_threads_per_pool;
		thread_within_pool = i % facdata->num_threads_per_pool;
		facdata->tf_threads[i].thread_can_sieve =
			(thread_within_pool == 0 ||
			 thread_within_pool * num_siever_threads_per_pool / facdata->num_threads_per_pool !=
			 (thread_within_pool-1) * num_siever_threads_per_pool / facdata->num_threads_per_pool);
		facdata->tf_threads[i].last_sieve_area_used = thread_within_pool;
	}

/* We often must allocate more sievers than siever threads.  For example, if num_siever_threads is 7, and */
/* we allocate 7 sievers with a facdist of 120 * p * 7 -- then one of the sievers will always generate */
/* multiples of 7.  Since that won't work, we must allocate more than 7 sievers for us to have 7 active */
/* siever threads.  To take get a little extra efficiency, we round up the number of allocated sievers to */
/* a multiple of 6 (non-zero remainders of 7) or 60 (non-zero remainders of 7*11) or 720 (non-zero remainders of 7*11*13). */

	num_siever_threads = facdata->num_pools * num_siever_threads_per_pool;
	if (num_siever_threads == 1) {
		num_siever_groups = 1;
		facdata->num_sievers = 1;
	} else if (num_siever_threads <= 30) {
		num_siever_groups = (num_siever_threads + 5) / 6 * 7;
		facdata->num_sievers = (num_siever_threads + 5) / 6 * 6;
	} else if (num_siever_threads <= 360) {
		num_siever_groups = (num_siever_threads + 59) / 60 * 77;
		facdata->num_sievers = (num_siever_threads + 59) / 60 * 60;
	} else {
		num_siever_groups = (num_siever_threads + 719) / 720 * 1001;
		facdata->num_sievers = (num_siever_threads + 719) / 720 * 720;
	}

	// It will be more efficient to always run with 7 or more siever allocs.  The one exception
	// is when doing QA finding known factors.
	if (facdata->num_sievers < 6 && IniGetInt (INI_FILE, "UseMaxSieverAllocs", 3) == 1)
		num_siever_groups = 7, facdata->num_sievers = 6;
	// I think it will be more efficient to always run with 60 or more siever allocs.  I saw a small
	// speed increase on Skylake due to the 9% reduction in sieving at the cost of more memory consumed.
	if (facdata->num_sievers < 60 && IniGetInt (INI_FILE, "UseMaxSieverAllocs", 3) == 2)
		num_siever_groups = 77, facdata->num_sievers = 60;
	// Increasing allocated sievers to 720 may be pushing it.  I need to benchmark this.
	// It is faster on Skylake, made it the default.
	if (facdata->num_sievers < 720 && IniGetInt (INI_FILE, "UseMaxSieverAllocs", 3) == 3)
		num_siever_groups = 1001, facdata->num_sievers = 720;

	// Calculate with small primes will be cleared by the initialization of the sieve area.
	if (num_siever_groups % 13 == 0) facdata->initsieve_primes = INITSIEVE_PRIMES4;
	else if (num_siever_groups % 11 == 0) facdata->initsieve_primes = INITSIEVE_PRIMES3;
	else if (num_siever_groups % 7 == 0) facdata->initsieve_primes = INITSIEVE_PRIMES2;
	else facdata->initsieve_primes = INITSIEVE_PRIMES1;

/* Generate the array of primes for sieving */

	{
		int	prime;			// Prime to add to the primes array
		uint32_t *primearray;		// Array of primes we are building
		int	maxprime;		// Maximum small prime we will sieve (for tuning)
		int	estimated_num_primes;	// Estimated number of primes below maxprime
		int	stop_reason;		// Error code from sieve initialization
		void	*sieve_info;		// Handle to small primes sieving data

		maxprime = IniGetInt (INI_FILE, "MaxTFSievePrime",
				      (CPU_FLAGS & CPU_AVX512F) ? 155000 :	// Knights Landing
				      (CPU_FLAGS & CPU_FMA3) ? 145000 :		// Skylake optimized
				      1000000);					// Need more data here
		if (maxprime <= 5000) maxprime = 5000;				// Enforce sensible limits on MaxTFSievePrime
		if (maxprime >= 100000000) maxprime = 100000000;		// Enforce sensible limits on MaxTFSievePrime
		estimated_num_primes = (int) ((double) maxprime / (log ((double) maxprime) - 1.0) * 1.01);
		asm_data->primearray = malloc (estimated_num_primes * sizeof (uint32_t));
		if (asm_data->primearray == NULL) goto memerr;
		primearray = (uint32_t *) asm_data->primearray;
		sieve_info = NULL;  /* Allocate a new sieve_info */
		stop_reason = start_sieve_with_limit (thread_num, 7, (int) sqrt ((double) maxprime) + 1, &sieve_info);
		if (stop_reason) return (stop_reason);
		for (num_small_primes = 0; num_small_primes < estimated_num_primes; ) {
			prime = (int) sieve (sieve_info);
			if (prime > maxprime) break;
			if (prime == p) continue;				// Rare case: TF where exponent <= maxprime
			if (num_siever_groups % prime == 0) continue;		// Don't add primes that are handled by facdist
			if (facdata->initsieve_primes % prime == 0) continue;	// Don't add primes that are handled by initsieve
			*primearray++ = prime;	// Save the small prime
			num_small_primes++;
		}
		*primearray = 0;		// Terminate the prime array
		end_sieve (sieve_info);
	}

/* Pre-calculate useful constants the assembly code will need */

	for (i = 0; i < 65; i++) asm_data->facdists[i] = i * 120 * (uint64_t) num_siever_groups * (uint64_t) p; // Compute multiples of 120 * p.
	asm_data->facdist12K = (SIEVE_SIZE_IN_BYTES * 8) * 120 * (uint64_t) num_siever_groups * (uint64_t) p; // Distance between first factor in sieve areas

/* Allocate and initialize the modular inverse of all the sieving primes */

	facdata->modinvarray = (uint32_t *) malloc (num_small_primes * sizeof (uint32_t));
	if (facdata->modinvarray == NULL) goto memerr;
	for (i = 0; i < num_small_primes; i++)
		facdata->modinvarray[i] = (uint32_t) modinv (asm_data->facdists[1], ((uint32_t *)asm_data->primearray)[i]);

/* Allocate the bit-to-clear array for each allocated siever */

	mem_needed = facdata->num_sievers * sizeof (struct siever_info);
	facdata->sievers = (struct siever_info *) malloc (mem_needed);
	if (facdata->sievers == NULL) goto memerr;
	memset (facdata->sievers, 0, mem_needed);
	for (i = 0; i < facdata->num_sievers; i++) {
		facdata->sievers[i].offsetarray = malloc (num_small_primes * sizeof (uint32_t));
		if (facdata->sievers[i].offsetarray == NULL) goto memerr;
	}

/* Allocate more memory for sieve area(s) and sieve initialization in each pool. */

	if (!facdata->pooling)
		facdata->num_sieve_areas_per_pool = facdata->num_threads_per_pool;
	else
		facdata->num_sieve_areas_per_pool = facdata->num_threads_per_pool * IniGetInt (INI_FILE, "TFSieveAreasPerThread", 4);
	mem_needed = facdata->num_sieve_areas_per_pool * sizeof (struct sieve_area_info);
	for (i = 0; i < facdata->num_pools; i++) {
		int	j;
		facdata->pools[i].sieve_areas = (struct sieve_area_info *) malloc (mem_needed);
		if (facdata->pools[i].sieve_areas == NULL) goto memerr;
		memset (facdata->pools[i].sieve_areas, 0, mem_needed);
		for (j = 0; j < facdata->num_sieve_areas_per_pool; j++) {
			facdata->pools[i].sieve_areas[j].sieve = aligned_malloc (SIEVE_SIZE_IN_BYTES, 128);
			if (facdata->pools[i].sieve_areas[j].sieve == NULL) goto memerr;
			facdata->pools[i].sieve_areas[j].state = SIEVE_AREA_FREE;
		}
	}
			
/* Allocate and initialize the bytes used to initialize a sieve */

	asm_data->initsieve = malloc (facdata->initsieve_primes + SIEVE_SIZE_IN_BYTES);
	if (asm_data->initsieve == NULL) goto memerr;
	memset (asm_data->initsieve, 0xFF, facdata->initsieve_primes + SIEVE_SIZE_IN_BYTES);
	for (i = 7; i <= 31; i += 2) {
		unsigned int j;
		if (facdata->initsieve_primes % i) continue;
		for (j = 0; j < (facdata->initsieve_primes + SIEVE_SIZE_IN_BYTES) * 8; j += i) bitclr (asm_data->initsieve, j);
	}

/* Each bit in the sieve represents one facdist.  We need to find the multiple of 8 (byte boundary) */
/* to start copying data from the sieve initializer array.  This modular inverse lets us do that. */

	facdata->one_eighth_initsieve_primes = (uint32_t) modinv (8 * asm_data->facdists[1], facdata->initsieve_primes);

/* Init mutexes and events used to control auxiliary threads */

	gwmutex_init (&facdata->thread_lock);
	gwevent_init (&facdata->thread_work_to_do);
	gwevent_init (&facdata->sieved_areas_available);
	gwevent_init (&facdata->all_threads_started);
	gwevent_init (&facdata->all_threads_done);
	gwevent_signal (&facdata->all_threads_done);

/* Pre-create each auxiliary thread used in TF code. */
/* We allocate the memory here so that error recovery is easier. */

	facdata->threads_must_exit = FALSE;
	for (i = 1; i < facdata->num_threads; i++) {
		struct factor_thread_data *info;

		info = (struct factor_thread_data *) malloc (sizeof (struct factor_thread_data));
		if (info == NULL) return (GWERROR_MALLOC);
		info->facdata = facdata;
		info->thread_num = i;
		/* Allocate the asm_data area - suitably aligned for future AVX-1024 variables */
		info->asm_data = (struct facasm_data *) aligned_malloc (sizeof (struct facasm_data), 128);
		if (info->asm_data == NULL) {
			free (info);
			return (GWERROR_MALLOC);
		}
		/* Launch the auxiliary thread */
		gwthread_create_waitable (&facdata->tf_threads[i].thread_id, &factor_auxiliary_thread, info);
	}

/* Setup complete */

	return (0);
}

/* Cleanup after making a factoring run */

void factorDone (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	struct facasm_data *asm_data = facdata->asm_data;
	int	i, j;

/* If we are multithreading and multithreading was initialized, then do multithreading cleanup */

	if (facdata->num_threads > 1 && facdata->thread_lock != NULL) {

/* Set termination variable and fire up auxiliary threads */

		facdata->threads_must_exit = TRUE;
		gwevent_signal (&facdata->thread_work_to_do);
		gwevent_signal (&facdata->sieved_areas_available);

/* Wait for all the threads to exit.  We must do this so */
/* that this thread can safely delete the facdata structure */

		for (i = 1; i < facdata->num_threads; i++)
			if (facdata->tf_threads[i].thread_id)
				gwthread_wait_for_exit (&facdata->tf_threads[i].thread_id);
	}

/* Free up memory */

	free (facdata->tf_threads);
	facdata->tf_threads = NULL;

/* Now free up the multithread resources */

	gwmutex_destroy (&facdata->thread_lock);
	gwevent_destroy (&facdata->thread_work_to_do);
	gwevent_destroy (&facdata->sieved_areas_available);
	gwevent_destroy (&facdata->all_threads_started);
	gwevent_destroy (&facdata->all_threads_done);
	facdata->thread_lock = NULL;

/* Free assembly code work area */

	if (asm_data != NULL) {
		free (asm_data->initsieve);
		free (asm_data->primearray);
		aligned_free (asm_data);
		facdata->asm_data = NULL;
	}
	free (facdata->modinvarray);
	facdata->modinvarray = NULL;

/* Free the allocated sievers */

	if (facdata->sievers != NULL) {
		for (i = 0; i < facdata->num_sievers; i++)
			free (facdata->sievers[i].offsetarray);
		free (facdata->sievers);
		facdata->sievers = NULL;
	}

/* Free the pools and sieve areas */

	if (facdata->pools != NULL) {
		for (i = 0; i < facdata->num_pools; i++) {
			if (facdata->pools[i].sieve_areas != NULL) {
				for (j = 0; j < facdata->num_sieve_areas_per_pool; j++)
					aligned_free (facdata->pools[i].sieve_areas[j].sieve);
				free (facdata->pools[i].sieve_areas);
			}
		}
		free (facdata->pools);
		facdata->pools = NULL;
	}
}

/* Prepare for one of the 16 factoring passes */
/* We figure out the starting factor for each siever, and delay much of the initialization */
/* for factorPassSetupPart2 when multithreading is at work. */

void add96_64 (uint64_t *a_hi, uint64_t *a_lo, uint64_t b) { *a_lo += b; *a_hi += (*a_lo < b) ? 1 : 0; }
uint32_t mod96_32 (uint64_t a_hi, uint64_t a_lo, uint32_t b) { return ((uint32_t) (((((((a_hi % b) << 32) + (a_lo >> 32)) % b) << 32) + (a_lo & 0xFFFFFFFF)) % b)); }

int factorPassSetup (
	int	thread_num,
	unsigned long pass,
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	struct facasm_data *asm_data = facdata->asm_data;
	uint32_t fachsw, facmsw;
	unsigned int i, j, bits_in_factor;

/* Save the factoring pass */

	facdata->factoring_pass = pass;

/* FACHSW/FACMSW specifies a floor for the first factor */
/* Start sieving at or above 2^44 (we brute force below 2^44) */

	fachsw = asm_data->FACHSW;
	facmsw = asm_data->FACMSW;
	if (fachsw == 0 && facmsw < 0x1000) facmsw = 0x1000;

/* Compute the number of bits in the factors we will be testing */

	if (fachsw) i = fachsw, bits_in_factor = 64;
	else i = facmsw, bits_in_factor = 32;
	while (i) bits_in_factor++, i >>= 1;
	if (bits_in_factor < 50) bits_in_factor = 50;		// PrimeFactor does 2^44 to 2^50 in one pass

/* Initialization for AVX512 FMA code using 2 doubles to factor numbers 45 bits and above. */
/* We need to initialize the following data: */
/*	FMA_SHIFTER		DD	32 DUP (0)	; Up to 32 dword shifter values
	ZMM_FMA_LOW_MASK	DQ	0		; Mask for low bits
	ZMM_FMA_RND_CONST0	DQ	0		; 3 * 2^(51 + SPLIT_POINT)
	ZMM_FMA_RND_CONST1	DQ	0		; 3 * 2^(51 + BITS_IN_LO_WORD*2)
	ZMM_FMA_RND_CONST2	DQ	0		; 3 * 2^(51 + BITS_IN_LO_WORD)
	ZMM_FMA_RND_CONST4	DQ	0		; 3 * 2^(51 - SPLIT_POINT)
	ZMM_FMA_RND_CONST5	DQ	0		; 3 * 2^(51 + BITS_IN_LO_WORD - SPLIT_POINT)
	ZMM_FMA_INITVAL		DQ	0		; Initial squared value (a power of two)
	ZMM_FMA_TWOPOW_SPLIT	DQ	0		; 2^SPLIT_POINT
	ZMM_FMA_TWO_TO_LO	DQ	0		; 2^BITS_IN_LO_WORD
*/

	if (bits_in_factor >= 45 && (asm_data->cpu_flags & CPU_AVX512F)) {
		uint32_t *dword_data;
		double	*zmm_data;
		uint64_t *zmm64_data;
		int	split_bits, low_bits;
		unsigned long p;

/* Calculate where we split rem_hi^2 (see ASM code in factor64.mac for full algorithm).  Calculate bits of precision in low double. */

		split_bits = bits_in_factor + (bits_in_factor + 1) / 2;
		low_bits = (split_bits - 52 + 1) / 2;

/* Set FMA_SHIFTER values */

		dword_data = (uint32_t *) asm_data->xmm_data;
		p = (unsigned long) asm_data->p;
		for (i = 0; p > 2 * bits_in_factor + 1; i++) {
			dword_data[i] = (p & 1) ? 64 : 0;		// FMA_SHIFTER (used to decide if this is a doubling iteration)
			p >>= 1;
		}

/* Compute the initial value and other constants. */

		asm_data->FMA_INITVAL_TYPE =
			((int) p < split_bits) ? -1 :
			((int) p > split_bits+1) ? 0 :
			((int) p == split_bits) ? 1 : 2;
		asm_data->FMA_64_MINUS_LOW_BITS = 64 - low_bits;
		asm_data->SSE2_LOOP_COUNTER = i;

		zmm64_data = (uint64_t *) &dword_data[32];
		zmm64_data[0] = (1ULL << low_bits) - 1; /* ZMM_FMA_LOW_MASK */
		zmm_data = (double *) &zmm64_data[1];
		zmm_data[0] = 3.0 * pow ((double) 2.0, (51 + split_bits)); /* ZMM_FMA_RND_CONST0 */
		zmm_data[1] = 3.0 * pow ((double) 2.0, (51 + 2*low_bits)); /* ZMM_FMA_RND_CONST1 */
		zmm_data[2] = 3.0 * pow ((double) 2.0, (51 + low_bits)); /* ZMM_FMA_RND_CONST2 */
		zmm_data[3] = 3.0 * pow ((double) 2.0, (51 - split_bits)); /* ZMM_FMA_RND_CONST4 */
		zmm_data[4] = 3.0 * pow ((double) 2.0, (51 + low_bits - split_bits)); /* ZMM_FMA_RND_CONST5 */
		if ((int) p > split_bits+1)
			zmm_data[5] = pow ((double) 2.0, (double) (p-1)); /* ZMM_FMA_INITIAL_VALUE */
		else
			zmm_data[5] = pow ((double) 2.0, (double) p); /* ZMM_FMA_INITIAL_VALUE */
		zmm_data[6] = pow ((double) 2.0, split_bits); /* ZMM_FMA_TWOPOW_SPLIT */
		zmm_data[7] = pow ((double) 2.0, low_bits); /* ZMM_FMA_TWO_TO_LO */
	}

/* Initialization for FMA code using 2 doubles to factor numbers 45 bits and above. */
/* We need to initialize the following data: */
/*	FMA_SHIFTER		DD	32 DUP (0)	; Up to 32 dword shifter values
	YMM_FMA_LOW_MASK	DQ	4 DUP		; Mask for low bits
	YMM_FMA_RND_CONST0	DQ	4 DUP		; 3 * 2^(51 + SPLIT_POINT)
	YMM_FMA_RND_CONST1	DQ	4 DUP		; 3 * 2^(51 + BITS_IN_LO_WORD*2)
	YMM_FMA_RND_CONST2	DQ	4 DUP		; 3 * 2^(51 + BITS_IN_LO_WORD)
	YMM_FMA_RND_CONST4	DQ	4 DUP		; 3 * 2^(51 - SPLIT_POINT)
	YMM_FMA_RND_CONST5	DQ	4 DUP		; 3 * 2^(51 + BITS_IN_LO_WORD - SPLIT_POINT)
	YMM_FMA_INITVAL		DQ	4 DUP		; Initial squared value (a power of two)
	YMM_FMA_TWOPOW_SPLIT	DQ	4 DUP		; 2^SPLIT_POINT
	YMM_FMA_TWO_TO_LO	DQ	4 DUP		; 2^BITS_IN_LO_WORD
*/

	else if (bits_in_factor >= 45 && (asm_data->cpu_flags & CPU_FMA3)) {
		uint32_t *dword_data;
		double	*ymm_data;
		uint64_t *ymm64_data;
		int	split_bits, low_bits;
		unsigned long p;

/* Calculate where we split rem_hi^2 (see ASM code in factor64.mac for full algorithm).  Calculate bits of precision in low double. */

		split_bits = bits_in_factor + (bits_in_factor + 1) / 2;
		low_bits = (split_bits - 52 + 1) / 2;

/* Set FMA_SHIFTER values */

		dword_data = (uint32_t *) asm_data->xmm_data;
		p = (unsigned long) asm_data->p;
		for (i = 0; p > 2 * bits_in_factor + 1; i++) {
			dword_data[i] = (p & 1) ? 32 : 0;		// FMA_SHIFTER (used to decide if this is a doubling iteration)
			p >>= 1;
		}

/* Compute the initial value and other constants. */

		asm_data->FMA_INITVAL_TYPE =
			((int) p < split_bits) ? -1 :
			((int) p > split_bits+1) ? 0 :
			((int) p == split_bits) ? 1 : 2;
		asm_data->FMA_64_MINUS_LOW_BITS = 64 - low_bits;
		asm_data->SSE2_LOOP_COUNTER = i;

		ymm64_data = (uint64_t *) &dword_data[32];
		ymm64_data[0] = ymm64_data[1] = ymm64_data[2] = ymm64_data[3] = (1ULL << low_bits) - 1; /* YMM_FMA_LOW_MASK */
		ymm_data = (double *) &ymm64_data[4];
		ymm_data[0] = ymm_data[1] = ymm_data[2] = ymm_data[3] = 3.0 * pow ((double) 2.0, (51 + split_bits)); /* YMM_FMA_RND_CONST0 */
		ymm_data[4] = ymm_data[5] = ymm_data[6] = ymm_data[7] = 3.0 * pow ((double) 2.0, (51 + 2*low_bits)); /* YMM_FMA_RND_CONST1 */
		ymm_data[8] = ymm_data[9] = ymm_data[10] = ymm_data[11] = 3.0 * pow ((double) 2.0, (51 + low_bits)); /* YMM_FMA_RND_CONST2 */
		ymm_data[12] = ymm_data[13] = ymm_data[14] = ymm_data[15] = 3.0 * pow ((double) 2.0, (51 - split_bits)); /* YMM_FMA_RND_CONST4 */
		ymm_data[16] = ymm_data[17] = ymm_data[18] = ymm_data[19] = 3.0 * pow ((double) 2.0, (51 + low_bits - split_bits)); /* YMM_FMA_RND_CONST5 */
		if ((int) p > split_bits+1)
			ymm_data[20] = ymm_data[21] = ymm_data[22] = ymm_data[23] = pow ((double) 2.0, (double) (p-1)); /* YMM_FMA_INITIAL_VALUE */
		else
			ymm_data[20] = ymm_data[21] = ymm_data[22] = ymm_data[23] = pow ((double) 2.0, (double) p); /* YMM_FMA_INITIAL_VALUE */
		ymm_data[24] = ymm_data[25] = ymm_data[26] = ymm_data[27] = pow ((double) 2.0, split_bits); /* YMM_FMA_TWOPOW_SPLIT */
		ymm_data[28] = ymm_data[29] = ymm_data[30] = ymm_data[31] = pow ((double) 2.0, low_bits); /* YMM_FMA_TWO_TO_LO */
	}

/* Initialization for SSE2 / AVX2 code using 30-bit integers to factor numbers 64 bits and above */
/* If using the SSE2 or AVX2 factoring code, we need to initialize much of the following data: */
/*	YMM_INITVAL		DD	0,0,0,0,0,0,0,0
	XMM_INIT120BS		DD	0,0
	XMM_INITBS		DD	0,0
	XMM_BS			DD	0,0
	XMM_SHIFTER		DD	64 DUP (0) */

	else if (bits_in_factor >= 64 && (asm_data->cpu_flags & (CPU_SSE2 | CPU_AVX2))) {
		uint32_t *xmm_data;
		unsigned long p;

/* Set XMM_SHIFTER values (the first shifter value is not used). */
/* Also compute the initial value. */

		xmm_data = asm_data->xmm_data;
		p = (unsigned long) asm_data->p;
		for (i = 0; p > bits_in_factor + 59; i++) {
			xmm_data[16+i*2] = (p & 1) ? 1 : 0;
			p >>= 1;
		}
		xmm_data[0] = xmm_data[2] =			/* XMM_INITVAL & YMM_INITVAL */
		xmm_data[4] = xmm_data[6] = p >= 90 ? 0 : (1 << (p - 60));
		xmm_data[8] = 62 - (120 - bits_in_factor);	/* XMM_INIT120BS */
		xmm_data[10] = 62 - (p - bits_in_factor);	/* XMM_INITBS */
		xmm_data[12] = bits_in_factor - 61;		/* Set XMM_BS to 60 - (120 - fac_size + 1) as defined in factor64.mac */
		asm_data->SSE2_LOOP_COUNTER = i;
		asm_data->TWO_TO_FACSIZE_PLUS_62 = pow ((double) 2.0, (int) (bits_in_factor + 62));
	}

/* Determine the starting factor for each siever.  Also calculate total number of sieves we'll need to process. */

	facdata->total_num_areas_to_sieve = 0;
	for (i = 0; (int) i < facdata->num_sievers; i++) {
		uint64_t savefac0, savefac1;
		double	first_factor;

/* In first siever, use fachsw/facmsw to calculate first factor */

		if (i == 0) {
			uint64_t rem;
			uint32_t rems[16] = {1,7,17,23,31,41,47,49,71,73,79,89,97,103,113,119};

			// fachsw/facmsw specifies a floor for the first factor

			savefac0 = fachsw;
			savefac1 = ((uint64_t) facmsw) << 32;

			// Find next integer of the form 2kp+1
			rem = mod96_32 (savefac0, savefac1, (uint32_t) asm_data->p * 2);
			add96_64 (&savefac0, &savefac1, asm_data->p * 2 - rem + 1);

			// Now make sure we have the right remainder for this factoring pass
			for ( ; ; ) {
				if (mod96_32 (savefac0, savefac1, 120) == rems[pass]) break;
				add96_64 (&savefac0, &savefac1, asm_data->p * 2);
			}
		}

/* For remaining sievers, add 120 * p to the previous siever's first factor */

		else {
			savefac0 = facdata->sievers[i-1].next_sieve_first_factor[0];
			savefac1 = facdata->sievers[i-1].next_sieve_first_factor[1];
			add96_64 (&savefac0, &savefac1, 120 * asm_data->p);
		}

/* Make sure factor is not a multiple of 7/11/13 when facdist is also a multiple of 7/11/13 */

		for ( ; ; ) {
			if ((asm_data->facdists[1] % 7 == 0 && mod96_32 (savefac0, savefac1, 7) == 0) ||
			    (asm_data->facdists[1] % 11 == 0 && mod96_32 (savefac0, savefac1, 11) == 0) ||
			    (asm_data->facdists[1] % 13 == 0 && mod96_32 (savefac0, savefac1, 13) == 0))
				add96_64 (&savefac0, &savefac1, 120 * asm_data->p);
			else
				break;
		}

/* Remember the first factor, the initial offset to fill the sieve, and init siever state */

		facdata->sievers[i].next_sieve_first_factor[0] = savefac0;
		facdata->sievers[i].next_sieve_first_factor[1] = savefac1;
		facdata->sievers[i].state = SIEVER_UNINITIALIZED;

/* Calculate number of calls we will need to make to factor64_sieve.  Be careful when reading a save file, the first */
/* factor could be just over the next power of two meaning there are no areas to sieve. */

		first_factor = (double) savefac0 * pow (2.0, 64.0) + (double) savefac1;
		if (facdata->endpt > first_factor)
			facdata->sievers[i].num_areas_to_sieve = (uint64_t) ((facdata->endpt - first_factor) / (double) asm_data->facdist12K) + 1;
		else
			facdata->sievers[i].num_areas_to_sieve = 0;
		facdata->total_num_areas_to_sieve += facdata->sievers[i].num_areas_to_sieve;
	}

/* Now let assembly code do remaining pass initialization.  Archaic stuff that never got moved to C code. */

	asm_data->savefac0 = facdata->sievers[0].next_sieve_first_factor[0];
	asm_data->savefac1 = facdata->sievers[0].next_sieve_first_factor[1];
	factor64_pass_setup (asm_data);

/* Start each sieving-capable thread with a different siever */

	j = 0;
	for (i = 0; (int) i < facdata->num_threads; i++) {
		if (facdata->tf_threads[i].thread_can_sieve) {
			facdata->sievers[j].last_thread_used = i;
			facdata->sievers[j].state |= SIEVER_ASSIGNED;
			facdata->tf_threads[i].last_siever_used = j++;
		}
	}

/* Init sieve area counters, clear counter of chunks processed */

	for (i = 0; (int) i < facdata->num_pools; i++) {
		facdata->pools[i].num_sievers_active = 0;
		facdata->pools[i].num_free_sieve_areas = facdata->num_sieve_areas_per_pool;
		facdata->pools[i].num_sieved_sieve_areas = 0;
		facdata->pools[i].last_sieve_area_for_sieve = 0;
		facdata->pools[i].last_sieve_area_for_tf = 0;
	}
	facdata->num_chunks_TFed = 0;
	facdata->total_num_chunks_TFed = 0;
	facdata->total_num_chunks_to_TF = facdata->total_num_areas_to_sieve;
	facdata->pass_complete = FALSE;

/* Signal the auxiliary threads to resume working */
/* This will copy the main thread's now properly initialized asm_data to each auxiliary thread */

	if (facdata->num_threads > 1) {
		facdata->num_active_threads = 0;			// Set count of active auxiliary threads
		gwevent_reset (&facdata->all_threads_started);		// Clear all auxiliary threads started signal
		gwevent_reset (&facdata->all_threads_done);		// Clear all auxiliary threads done signal
		gwevent_signal (&facdata->thread_work_to_do);		// Start auxiliary threads
	}

/* Setup complete */

	return (0);
}

/* This is remaining initialization that each allocated siever must do before its first sieving operation this pass */
/* We don't do this in factorPassSetup because that is single-threaded.  This routine will be executed in multi-threaded. */
/* You might think this is no big deal, but on Knight's Landing where there are 64 or more siever areas, this single-threaded */
/* initialization cost adds substantial elapsed time when TFing at lower bit levels.  Also, why initialize the offset data */
/* in one thread only to use it in another thread -- can generate needless bus traffic. */

void factorPassSetupPart2 (
	fachandle *facdata,		/* Handle returned by factorSetup */
	struct facasm_data *asm_data,	/* This thread's asm data */
	struct siever_info *siever)	/* Siever to finish initializing */
{
	uint32_t *primep, *modinvp, *offsetp;

/* Init this siever's the bit-to-clear offset array */

	modinvp = facdata->modinvarray;
	offsetp = (uint32_t *) siever->offsetarray;
	for (primep = (uint32_t *) asm_data->primearray; *primep; primep++, offsetp++, modinvp++) {
		uint32_t temp;
		temp = mod96_32 (asm_data->savefac0, asm_data->savefac1, *primep);
		if (temp) temp = (uint32_t) (*primep - (((uint64_t) temp * (uint64_t) *modinvp) % *primep));
		*offsetp = temp;
	}
}

/* Factor one "chunk" single-threaded.  The assembly code decides how big a chunk is. */

#define FACTOR_CHUNK_SIZE	(SIEVE_SIZE_IN_BYTES >> 10)		// 64-bit sieve processes one 12KB sieve

int factorChunk (			/* Return 1 if factor found, 2 if factor not found */
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	struct facasm_data *asm_data = facdata->asm_data;
	int	res;

/* If we're looking for factors below 2^44, use special brute force code */

	if (facdata->factoring_pass == 0 && asm_data->FACHSW == 0 && asm_data->FACMSW < 0x1000) {
		asm_data->savefac1 = 2 * (uint64_t) asm_data->p + 1;		// First factor to test is 2*p+1
		while (factor64_small (asm_data) == 1) {			// Remember the found factor
			if (facdata->found_count < MAX_TF_FOUND_COUNT) {	// I doubt we'll find more than 10 factors
				facdata->found_lsw[facdata->found_count] = (uint32_t) asm_data->savefac1;
				facdata->found_msw[facdata->found_count] = (uint32_t) (asm_data->savefac1 >> 32);
				facdata->found_hsw[facdata->found_count] = 0;
				facdata->found_count++;
			}
			asm_data->savefac1 += 2 * (uint64_t) asm_data->p;	// Calculate next factor to test
		}
	}

/* The main thread uses the same code as the auxilary threads */

	res = factorChunkMultithreaded (facdata, asm_data, 0);
	// If we didn't TF a chunk (because there are no more to do) then wait
	// for the auxiliary threads to finish up, just in case one of them
	// finds a factor.
	if (res) {
		gwevent_wait (&facdata->all_threads_done, 0);
		facdata->pass_complete = TRUE;
	}
	// If we (or an auxiliary thread) found a factor, end this pass and return the factor
	if (facdata->found_count) {
		// Caller expects the pass to be complete when a found factor is returned.
		// We should clean this up, but it would require modifying the legacy 32-bit code.
		do {
			res = factorChunkMultithreaded (facdata, asm_data, 0);
		} while (!res);
		gwevent_wait (&facdata->all_threads_done, 0);
		facdata->pass_complete = TRUE;
		facdata->found_count--;
		asm_data->FACLSW = facdata->found_lsw[facdata->found_count];
		asm_data->FACMSW = facdata->found_msw[facdata->found_count];
		asm_data->FACHSW = facdata->found_hsw[facdata->found_count];
		return (1);					// Return factor found
	}
	return (2);						// Return factor not found
}

/* Like the code above, but a multithreaded version */

int factorChunkMultithreaded (		/* Return TRUE when there is no more work to do */
	fachandle *facdata,		/* Handle returned by factorSetup */
	struct facasm_data *asm_data,	/* This thread's asm data */
	int	aux_thread_num)		/* Auxiliary thread number (or zero for main thread) */
{
	int	i, res, siever_uninitialized;
	struct tf_thread_info *tf_thread;
	struct pool_info *pool;
	struct sieve_area_info *sieve_area;

/* Get info on this particular thread */

	tf_thread = &facdata->tf_threads[aux_thread_num];
	pool = &facdata->pools[tf_thread->pool];

/* Acquire the lock */

	gwmutex_lock (&facdata->thread_lock);

/* See if we should do some sieving.  The thread must be one that can sieve and there must be more sieve work left to do */

	if (facdata->total_num_areas_to_sieve > 0 &&				// Is any sieving needed?
	    (!facdata->pooling ||						// If not pooling, then we can always sieve
	     (tf_thread->thread_can_sieve &&					// Pooling: Is thread allowed to sieve?
	      pool->num_free_sieve_areas &&					// Pooling: Are there sieve areas free to work on?
	      pool->num_sieved_sieve_areas < facdata->num_threads_per_pool*2))){// Pooling: Are there adequate areas already sieved? */
		struct siever_info *siever;
		siever = &facdata->sievers[tf_thread->last_siever_used];

/* We prefer to use the same siever as last time because the bit-to-clear offsets are likely to */
/* be in one of the CPU's caches.  When we are not close to finishing up, we switch sievers after */
/* every 64 sievings so that we make close to uniform progress (which is prefered should we need */
/* to create a save file).  When we are close to finishing up, we want every thread to end at the */
/* same time so we become very strict about selecting neediest siever. */

		if (siever->last_thread_used == aux_thread_num &&		// Siever is assigned to this thread
		    (siever->state == SIEVER_ASSIGNED + SIEVER_UNINITIALIZED ||	// Siever hasn't started yet
		     ((siever->num_areas_to_sieve & 0x3F) &&			// Siever is in middle of 64 sievings
		      (siever->num_areas_to_sieve >= 20 ||			// and not near the end of the pass
		       facdata->num_threads == 1))))				// or only one thread: uniform progress at end is unimportant
			;							// Stay with our assigned siever

/* Find the inactive siever with the most work left to do. */
/* We fudge a little here so that we prefer to stay with the same siever. */

		else {
			int	i, best_siever;
			uint64_t score, best_score_thusfar;

			// Mark our old siever unassigned if the siever was assigned to this thread
			if (siever->last_thread_used == aux_thread_num) siever->state = SIEVER_INACTIVE;

			// If not multithreading, we simply move onto the next siever for uniform progress
			if (facdata->num_threads == 1) {
				best_siever = tf_thread->last_siever_used + 1;
				if (best_siever == facdata->num_sievers) best_siever = 0;
			}

			// Multithreaded case.  Use heuristics to prefer re-using our current siever or an
			// unassigned siever except when we get near the end of a pass.
			else {
				best_siever = -1;
				if (siever->last_thread_used == aux_thread_num) {
					best_siever = tf_thread->last_siever_used;
					best_score_thusfar = siever->num_areas_to_sieve;
					if (siever->num_areas_to_sieve > 128) best_score_thusfar += 128;
					else if (siever->num_areas_to_sieve > 64) best_score_thusfar += 64;
				}
				for (i = 0; i < facdata->num_sievers; i++) {
					// We cannot steal a siever that is actively sieving
					if (facdata->sievers[i].state == SIEVER_ASSIGNED + SIEVER_ACTIVE) continue;
					// We don't steal sievers until we get very close to finishing
					if ((facdata->sievers[i].state & SIEVER_ASSIGNED) &&
					    facdata->sievers[i].num_areas_to_sieve > 20) continue;
					// Does this inactive siever have more work left to do than any sievers we've looked at thusfar?
					score = facdata->sievers[i].num_areas_to_sieve;
					if (best_siever == -1 || score > best_score_thusfar) {
						best_siever = i;
						best_score_thusfar = score;
					}
				}
			}

			// Assign the best siever to this thread
			tf_thread->last_siever_used = best_siever;
			siever = &facdata->sievers[best_siever];
			siever->state |= SIEVER_ASSIGNED;
			siever->last_thread_used = aux_thread_num;
		}

/* It is possible to get here with a siever that has no work to do (all sievers with work */
/* to do are currently tied up sieving).  Because we try to make uniform progress for */
/* the last 20 sievings, this should be pretty darn rare.  */

		if (siever->num_areas_to_sieve == 0) {
			if (facdata->pooling && pool->num_sieved_sieve_areas) goto tf;
			gwevent_reset (&facdata->sieved_areas_available);
			gwmutex_unlock (&facdata->thread_lock);
			gwevent_wait (&facdata->sieved_areas_available, 0);
			return (FALSE);
		}

/* Find a free sieve area and do the small prime sieving. */

		pool->num_sievers_active++;
		siever_uninitialized = (siever->state == SIEVER_ASSIGNED + SIEVER_UNINITIALIZED);
		siever->state = SIEVER_ASSIGNED + SIEVER_ACTIVE;
		siever->num_areas_to_sieve--;
		facdata->total_num_areas_to_sieve--;
		// If we can use the same sieve area as last time, do so for better locality.
		// When we are not really pooling, this will always be the case.
		if (pool->sieve_areas[tf_thread->last_sieve_area_used].state == SIEVE_AREA_FREE)
			i = tf_thread->last_sieve_area_used;
		// Otherwise, find the next free sieve area
		else for (i = pool->last_sieve_area_for_sieve + 1; ; i++) {
			if (i == facdata->num_sieve_areas_per_pool) i = 0;
			if (pool->sieve_areas[i].state == SIEVE_AREA_FREE) break;
		}
		tf_thread->last_sieve_area_used = i;
		pool->last_sieve_area_for_sieve = i;
		sieve_area = &pool->sieve_areas[i];
		pool->num_free_sieve_areas--;
		sieve_area->state = SIEVE_AREA_SIEVING;		/* Update sieve area's state */
		gwmutex_unlock (&facdata->thread_lock);
		sieve_area->first_factor[0] = asm_data->savefac0 = siever->next_sieve_first_factor[0];
		sieve_area->first_factor[1] = asm_data->savefac1 = siever->next_sieve_first_factor[1];
		asm_data->initstart = (uint32_t)
			((uint64_t) mod96_32 (asm_data->savefac0, asm_data->savefac1, facdata->initsieve_primes) *
			 (uint64_t) facdata->one_eighth_initsieve_primes % facdata->initsieve_primes);
		asm_data->sieve = sieve_area->sieve;
		asm_data->offsetarray = siever->offsetarray;
		if (siever_uninitialized) factorPassSetupPart2 (facdata, asm_data, siever);
		factor64_sieve (asm_data);
		gwmutex_lock (&facdata->thread_lock);
		siever->next_sieve_first_factor[0] = asm_data->savefac0; /* Remember next sieve's first factor */
		siever->next_sieve_first_factor[1] = asm_data->savefac1;
		sieve_area->state = SIEVE_AREA_SIEVED;
		pool->num_sieved_sieve_areas++;
		gwevent_signal (&facdata->sieved_areas_available);
		siever->state = SIEVER_ASSIGNED + SIEVER_INACTIVE;
		pool->num_sievers_active--;

/* If we are pooling, then return since we did some work. */
/* Otherwise, fall through and TF the area we just sieved. */

		if (facdata->pooling) {
			gwmutex_unlock (&facdata->thread_lock);
			return (FALSE);
		}
	}

/* See if there are any sieved areas available to TF.  If not, wait or exit */

	while (pool->num_sieved_sieve_areas == 0) {
		if ((!facdata->pooling || pool->num_sievers_active == 0) && facdata->total_num_areas_to_sieve == 0) {
			gwmutex_unlock (&facdata->thread_lock);
			return (TRUE);			// Return no more work to do flag
		}
		gwevent_reset (&facdata->sieved_areas_available);
		gwmutex_unlock (&facdata->thread_lock);
		gwevent_wait (&facdata->sieved_areas_available, 0);
		if (facdata->threads_must_exit) return (TRUE);
		gwmutex_lock (&facdata->thread_lock);
	}

/* Find a sieved area and TF it. */

	// If not pooling then use the same sieve area as last time for better locality.
tf:	if (!facdata->pooling)
		i = tf_thread->last_sieve_area_used;
	// Otherwise, find the sieved area with smallest first factor.  Well, rather than find the smallest
	// we just circle through the array by starting one after the last index used.  It won't take long
	// before the smallest sieved area is handed out for TF. */
	else for (i = pool->last_sieve_area_for_tf+1; ; i++) {
		if (i == facdata->num_sieve_areas_per_pool) i = 0;
		if (pool->sieve_areas[i].state == SIEVE_AREA_SIEVED) break;
	}
	tf_thread->last_sieve_area_used = i;
	pool->last_sieve_area_for_tf = i;
	sieve_area = &pool->sieve_areas[i];
	pool->num_sieved_sieve_areas--;
	sieve_area->state = SIEVE_AREA_TFING;			/* Update sieve area's state */
	asm_data->savefac0 = sieve_area->first_factor[0];
	asm_data->savefac1 = sieve_area->first_factor[1];
	asm_data->sieve = sieve_area->sieve;
	gwmutex_unlock (&facdata->thread_lock);
	res = factor64_tf (asm_data);
	gwmutex_lock (&facdata->thread_lock);
	if (res == 1) {							/* Remember a found factor */
		// On first found factor, only sieve areas below the found factor
		if (facdata->found_count == 0 && !IniGetInt (INI_FILE, "TFFullBitLevel", 0)) {
			double factor = ((double) asm_data->FACHSW * 4294967296.0 + (double) asm_data->FACMSW) * 4294967296.0;
			uint64_t reduction = (uint64_t) ((facdata->endpt - factor) / (double) asm_data->facdist12K);
			for (i = 0; i < facdata->num_sievers; i++) {
				if (facdata->sievers[i].num_areas_to_sieve >= reduction) {
					facdata->total_num_chunks_to_TF -= reduction;
					facdata->total_num_areas_to_sieve -= reduction;
					facdata->sievers[i].num_areas_to_sieve -= reduction;
				} else {
					facdata->total_num_chunks_to_TF -= facdata->sievers[i].num_areas_to_sieve;
					facdata->total_num_areas_to_sieve -= facdata->sievers[i].num_areas_to_sieve;
					facdata->sievers[i].num_areas_to_sieve = 0;
				}
			}
			gwevent_signal (&facdata->sieved_areas_available); // In case a thread was waiting for a siever we just zeroed
		}
		// Remember the found factor
		if (facdata->found_count < MAX_TF_FOUND_COUNT) {	/* I can't imagine finding too many factors so close together */
			facdata->found_lsw[facdata->found_count] = asm_data->FACLSW;
			facdata->found_msw[facdata->found_count] = asm_data->FACMSW;
			facdata->found_hsw[facdata->found_count] = asm_data->FACHSW;
			facdata->found_count++;
		}
	}
	facdata->num_chunks_TFed++;
	facdata->total_num_chunks_TFed++;
	sieve_area->state = SIEVE_AREA_FREE;			/* Update sieve area's state */
	pool->num_free_sieve_areas++;
	gwmutex_unlock (&facdata->thread_lock);
	return (FALSE);							// Return more work to do flag
}

/* Number of chunks that factorChunk processed.  This can be more than one when multithreading in 64-bit code */

int factorChunksProcessed (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	int	res;
	res = facdata->num_chunks_TFed;
	facdata->num_chunks_TFed = 0;		// Yes, we could have a race condition here, but it does not really matter
	return (res);
}

/* Find sieved area with smallest first factor so that we can write a save file */

void factorFindSmallestNotTFed (
	fachandle *facdata)		/* Handle returned by factorSetup */
{
	int	i, j;
	uint64_t smallest, temp;
	struct facasm_data *asm_data = facdata->asm_data;

/* Search for the smallest sieved area.  If there are no sieved areas, we'll end up */
/* returning the next factor to sieve. */

	gwmutex_lock (&facdata->thread_lock);
	for (i = 0; i < facdata->num_sievers; i++) {
		temp = (facdata->sievers[i].next_sieve_first_factor[0] << 32) +
		       (facdata->sievers[i].next_sieve_first_factor[1] >> 32);
		if (i == 0 || temp < smallest) smallest = temp;
	}
	for (i = 0; i < facdata->num_pools; i++) {
		for (j = 0; j < facdata->num_sieve_areas_per_pool; j++) {
			if (facdata->pools[i].sieve_areas[j].state != SIEVE_AREA_SIEVED) continue;
			temp = (facdata->pools[i].sieve_areas[j].first_factor[0] << 32) +
			       (facdata->pools[i].sieve_areas[j].first_factor[1] >> 32);
			if (temp < smallest) smallest = temp;
		}
	}

/* Return the information in FACHSW, FACMSW. */

	asm_data->FACHSW = (uint32_t) (smallest >> 32);
	asm_data->FACMSW = (uint32_t) smallest;
	gwmutex_unlock (&facdata->thread_lock);
}

/* Return a second or third found factor.  Should be extremely rare.  Can only happen in multithreaded case. */

int getAnotherFactorIfAny (
	fachandle *facdata)
{
	if (facdata->num_threads <= 1 || facdata->found_count == 0) return (FALSE);
	facdata->found_count--;
	facdata->asm_data->FACLSW = facdata->found_lsw[facdata->found_count];
	facdata->asm_data->FACMSW = facdata->found_msw[facdata->found_count];
	facdata->asm_data->FACHSW = facdata->found_hsw[facdata->found_count];
	return (TRUE);
}

#endif



/* Trial factor a Mersenne number prior to running a Lucas-Lehmer test */

static const char FACMSG[] = "Trial factoring M%ld to 2^%d is %.*f%% complete.";
static const char SHORT_FACMSG[] = "Trial factoring M%ld to 2^%d.";

#define FACTOR_MAGICNUM		0x1567234D
#define FACTOR_VERSION		1

int primeFactor (
	int	thread_num,
	struct PriorityInfo *sp_info,	/* SetPriority information */
	struct work_unit *w,
	unsigned int factor_limit_adjustment)
{
	fachandle facdata;		/* Handle to the factoring data */
	unsigned long p;		/* Exponent to factor */
	unsigned long bits;		/* How far already factored in bits */
	unsigned long test_bits;	/* How far to factor to */
	long	factor_found;		/* Returns true if factor found */
	int	fd;			/* Continuation file handle or zero */
	int	first_iter_msg, continuation, stop_reason, find_smaller_factor;
	unsigned long endpthi, endptlo;
	double	endpt, startpt;		/* For computing percent complete */
	unsigned long pass;		/* Factoring pass 0 through 15 */
	unsigned long report_bits;	/* When to report results one bit */
					/* at a time */
	readSaveFileState read_save_file_state; /* Manage savefile names during reading */
	writeSaveFileState write_save_file_state; /* Manage savefile names during writing */
	char	filename[32];
	char	buf[200], str[80];
	double	timers[2];

/* Init */

begin:	factor_found = 0;
	p = w->n;
	bits = (unsigned int) w->sieve_depth;

/* Determine how much we should factor (in bits) */

	test_bits = (unsigned int) w->factor_to - factor_limit_adjustment;

/* Is exponent already factored enough? This should never happen with */
/* WORK_FACTOR work units.  However, I suppose the user could have */
/* manually changed the line in worktodo.ini.  So send a message to */
/* server saying we didn't do any factoring but we are done with */
/* this work unit.  Then delete the work unit. */

	if (bits >= test_bits) {
		if (w->work_type == WORK_FACTOR) {
			struct primenetAssignmentResult pkt;
			memset (&pkt, 0, sizeof (pkt));
			strcpy (pkt.computer_guid, COMPUTER_GUID);
			strcpy (pkt.assignment_uid, w->assignment_uid);
			pkt.result_type = PRIMENET_AR_TF_NOFACTOR;
			pkt.n = p;
			pkt.done = TRUE;
			spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
			return (STOP_WORK_UNIT_COMPLETE);
		}
		return (0);
	}

/* Setup the factoring code */

	if (HYPERTHREAD_TF) sp_info->normal_work_hyperthreads = IniGetInt (LOCALINI_FILE, "HyperthreadTFcount", CPU_HYPERTHREADS);
	facdata.num_threads = CORES_PER_TEST[thread_num] * sp_info->normal_work_hyperthreads;
	facdata.sp_info = sp_info;
	stop_reason = factorSetup (thread_num, p, &facdata);
	if (stop_reason) {
		factorDone (&facdata);
		return (stop_reason);
	}

/* Record the amount of memory being used by this thread (1MB). */

	set_memory_usage (thread_num, 0, 1);

/* Note: The non-SSE2 code cannot handle factoring 79 bits and above. */

	if (test_bits > 78 && !(facdata.asm_data->cpu_flags & (CPU_SSE2 | CPU_AVX2 | CPU_FMA3 | CPU_AVX512F))) {
		OutputBoth (thread_num, "Pre-SSE2 trial factoring code cannot go above 2^78.\n");
		test_bits = 78;
	}

/* Check for a v24 continuation file.  These were named pXXXXXXX.  The */
/* first 16 bits contained a 2 to distinguish it from a LL save file. */
/* In v25, we name the file fXXXXXXX and use the common header format */
/* to make Test/Status and computing completion dates easier. */

	continuation = FALSE;
	tempFileName (w, filename);
	filename[0] = 'p';
	fd = _open (filename, _O_BINARY | _O_RDONLY);
	if (fd > 0) {
		short	type;
		short	shortdummy;
		unsigned long longdummy, fachsw, facmsw;
		short	file_factor_found, file_bits, file_pass;

		if (read_short (fd, &type) &&
		    type == 2 &&
		    read_long (fd, &longdummy, NULL) &&
		    read_short (fd, &shortdummy) &&
		    read_short (fd, &file_factor_found) &&
		    read_short (fd, &shortdummy) &&
		    read_short (fd, &file_bits) &&
		    read_short (fd, &file_pass) &&
		    read_long (fd, &fachsw, NULL) &&
		    read_long (fd, &facmsw, NULL) &&
		    read_long (fd, &endpthi, NULL) &&
		    read_long (fd, &endptlo, NULL)) {
			OutputBoth (thread_num, "Using old-style factoring save file.\n");
			facdata.asm_data->FACHSW = fachsw;
			facdata.asm_data->FACMSW = facmsw;
			factor_found = file_factor_found;
			bits = file_bits;
			pass = file_pass;
			continuation = TRUE;
			_close (fd);
			_unlink (filename);
		} else {
			_close (fd);
		}
	}

/* Read v25+ continuation file.  Limit number of backup files we try */
/* to read in case there is an error deleting bad save files. */

	filename[0] = 'f';
	readSaveFileStateInit (&read_save_file_state, thread_num, filename);
	writeSaveFileStateInit (&write_save_file_state, filename, 0);
	for ( ; ; ) {

		if (! saveFileExists (&read_save_file_state)) {
			/* If there were save files, they are all bad.  Report a message */
			/* and temporarily abandon the work unit.  We do this in hopes that */
			/* we can successfully read one of the bad save files at a later time. */
			/* This sounds crazy, but has happened when OSes get in a funky state. */
			if (read_save_file_state.a_non_bad_save_file_existed) {
				OutputBoth (thread_num, ALLSAVEBAD_MSG);
				return (0);
			}
			/* No save files existed, start from scratch. */
			break;
		}

		fd = _open (read_save_file_state.current_filename, _O_BINARY | _O_RDONLY);
		if (fd > 0) {
			unsigned long version, sum, fachsw, facmsw;
			if (read_magicnum (fd, FACTOR_MAGICNUM) &&
			    read_header (fd, &version, w, &sum) &&
			    version == FACTOR_VERSION &&
			    read_long (fd, (unsigned long *) &factor_found, NULL) &&
			    read_long (fd, &bits, NULL) &&
			    read_long (fd, &pass, NULL) &&
			    read_long (fd, &fachsw, NULL) &&
			    read_long (fd, &facmsw, NULL) &&
			    read_long (fd, &endpthi, NULL) &&
			    read_long (fd, &endptlo, NULL) &&
			    (fachsw < endpthi || (fachsw == endpthi && facmsw < endptlo))) {
				facdata.asm_data->FACHSW = fachsw;
				facdata.asm_data->FACMSW = facmsw;
				continuation = TRUE;
			}
			_close (fd);
			if (continuation) break;
		}

		/* Close and rename the bad save file */
		saveFileBad (&read_save_file_state);
	}

/* Init the title */

	sprintf (buf, "Factoring M%ld", p);
	title (thread_num, buf);
	sprintf (buf, "%s trial factoring of M%ld to 2^%lu\n",
		 fd > 0 ? "Resuming" : "Starting", p, test_bits);
	OutputStr (thread_num, buf);

/* Clear all timers */

	clear_timers (timers, sizeof (timers) / sizeof (timers[0]));

/* When doing easy and quick trial factoring on a Mersenne number, */
/* do not send a message to the server for every bit level we complete. */
/* If we did, the client would spend more CPU time sending messages to the */
/* server than actually factoring numbers.  Here we calculate the threshold */
/* where we'll start reporting results one bit at time.  We've arbitrarily */
/* chosen the difficulty in trial factoring M100000000 to 2^64 as the */
/* point where it is worthwhile to report results one bit at a time. */

	report_bits = (unsigned long) (64.0 + _log2 ((double) p / 100000000.0));
	if (report_bits >= test_bits) report_bits = test_bits;

/* Loop testing larger and larger factors until we've tested to the */
/* appropriate number of bits.  Advance one bit at a time because it */
/* is faster to look for factors at lower bit levels first. */
/* We always enter this loop if there is a continuation file because v23 */
/* had higher factoring limits and if we upgrade to v25 midstream, we */
/* might not send a factoring complete message to the server if we don't */
/* finish off the current bit level. */

	while (test_bits > bits || continuation) {
	    unsigned int end_bits;
	    unsigned long iters, iters_r, iters_just_processed;

/* Advance one bit at a time to minimize wasted time looking for a */
/* second factor after a first factor is found. */

	    end_bits = (bits < 50) ? 50 : bits + 1;
	    if (end_bits > test_bits) end_bits = test_bits;
	    sprintf (w->stage, "TF%d", end_bits);

/* Compute the ending point for each pass */

	    if (!continuation) {
		if (end_bits < 64) {
			endpthi = 0;
			endptlo = 1L << (end_bits-32);
		} else {
			endpthi = 1L << (end_bits-64);
			endptlo = 0;
		}
	    }

/* Precompute some constant for calculating percent complete */

	    if (bits < 32) startpt = 0.0;
	    else startpt = pow ((double) 2.0, (int) (bits-32));
	    endpt = endpthi * 4294967296.0 + endptlo;

/* Sixteen passes.  Two for the 1 or 7 mod 8 factors times two for the */
/* 1 or 2 mod 3 factors times four for the 1, 2, 3, or 4 mod 5 factors. */

	    iters_r = 0;
	    iters = 0;
	    first_iter_msg = (continuation ? 1 : 2);
	    if (! continuation) pass = 0;
	    for ( ; pass < 16; pass++) {

/* Set the starting point only if we are not resuming from */
/* a continuation file.  For no particularly good reason we */
/* quickly redo trial factoring for factors below 2^50. */

		if (continuation)
			continuation = FALSE;
		else {
			if (bits < 50) {
				facdata.asm_data->FACHSW = 0;
				facdata.asm_data->FACMSW = 0;
			} else if (bits < 64) {
				facdata.asm_data->FACHSW = 0;
				facdata.asm_data->FACMSW = 1L << (bits-32);
			} else {
				facdata.asm_data->FACHSW = 1L << (bits-64);
				facdata.asm_data->FACMSW = 0;
			}
		}

/* Only test for factors less than 2^32 on the first pass */

		if (facdata.asm_data->FACHSW == 0 &&
		    facdata.asm_data->FACMSW == 0 && pass != 0)
			facdata.asm_data->FACMSW = 1;

/* Setup the factoring program.  factorPassSetup needs to know the endpt in the case where */
/* we've already found a factor and we are searching for a smaller factor -- factorPassSetup */
/* used to assume the endpt was the next power of two. */

		facdata.endpt = endpt * 4294967296.0;
		stop_reason = factorPassSetup (thread_num, pass, &facdata);
		if (stop_reason) {
			factorDone (&facdata);
			return (stop_reason);
		}

/* Loop until all factors tested or factor found */

		for ( ; ; ) {
			int	res;
			double	currentpt;

/* Do a chunk of factoring.  Usually this is just one chunk (as defined by the underlying factoring code). */
/* However, when multithreading the auxiliary threads are TFing chunks at the same time. */

			start_timer (timers, 0);
			res = factorChunk (&facdata);
			end_timer (timers, 0);

/* If we found a factor, verify it */

			if (res == 1) {
				stackgiant (f, 10);
				stackgiant (x, 10);
				itog ((int) facdata.asm_data->FACHSW, f); gshiftleft (32, f);
				uladdg (facdata.asm_data->FACMSW, f); gshiftleft (32, f);
				uladdg (facdata.asm_data->FACLSW, f);
				itog (2, x);
				powermod (x, p, f);

/* If factor was verified, break out of our factor search */

				if (isone (x)) break;

/* If factor is no good, print an error message, sleep, re-initialize and */
/* restart the factoring code. */

				OutputBoth (thread_num, "ERROR: Incorrect factor found.\n");
				factorDone (&facdata);
				stop_reason = SleepFive (thread_num);
				if (stop_reason) return (stop_reason);
				goto begin;
			}

/* Compute new percentage complete (of this bit level) */

#ifdef X86_64
			// Calc number of chunks left to do
			currentpt = (double) facdata.total_num_chunks_to_TF - (double) facdata.total_num_chunks_TFed;
			// Calc average number of chunks to do in each siever
			currentpt /= (double) facdata.num_sievers;
			// Calc distance from endpt
			currentpt *= (double) facdata.asm_data->facdist12K;
			// Calc currentpt scaled down by 2^32
			currentpt = endpt - currentpt / 4294967296.0;
#else
			currentpt = facdata.asm_data->FACHSW * 4294967296.0 + facdata.asm_data->FACMSW;
			if (currentpt > endpt) currentpt = endpt;
#endif
			w->pct_complete = (pass + (currentpt - startpt) / (endpt - startpt)) / 16.0;

/* Output informative message.  Usually this includes a percent complete, however, */
/* when just beginning a bit level (first_iter_msg == 2) we don't as the percentage */
/* is close to zero.  We define one iteration as the time it takes to process 1Mbit of sieve. */

			stop_reason = stopCheck (thread_num);
			iters_just_processed = factorChunksProcessed (&facdata);
			iters += iters_just_processed;
			if (((iters * FACTOR_CHUNK_SIZE) >> 7) >= ITER_OUTPUT || first_iter_msg) {
				double	pct;
				pct = trunc_percent (w->pct_complete);
				if (first_iter_msg == 2) {
					sprintf (buf, "M%ld to 2^%d", p, end_bits);
				} else {
					sprintf (buf, "%.*f%% of M%ld to 2^%d", (int) PRECISION, pct, p, end_bits);
				}
				title (thread_num, buf);
				if (first_iter_msg == 2) {
					sprintf (buf, SHORT_FACMSG, p, end_bits);
				} else {
					sprintf (buf, FACMSG, p, end_bits, (int) PRECISION, pct);
				}
				if (first_iter_msg) {
					strcat (buf, "\n");
					clear_timer (timers, 0);
				} else {
					strcat (buf, "  Time: ");
					print_timer (timers, 0, buf, TIMER_NL | TIMER_OPT_CLR);
				}
				OutputStr (thread_num, buf);
				iters = 0;
				first_iter_msg = FALSE;
			}

/* Output informative message */

			iters_r += iters_just_processed;
			if (((iters_r * FACTOR_CHUNK_SIZE) >> 10) >= ITER_OUTPUT_RES || (NO_GUI && stop_reason)) {
				sprintf (buf, FACMSG, p, end_bits, (int) PRECISION, trunc_percent (w->pct_complete));
				strcat (buf, "\n");
				writeResults (buf);
				iters_r = 0;
			}

/* If an escape key was hit, write out the results and return */

			if (stop_reason || testSaveFilesFlag (thread_num)) {
				factorFindSmallestNotTFed (&facdata);
				if (facdata.asm_data->FACHSW > endpthi ||
				    (facdata.asm_data->FACHSW == endpthi && facdata.asm_data->FACMSW >= endptlo)) {
					if (endptlo == 0) facdata.asm_data->FACHSW = endpthi - 1;
					else facdata.asm_data->FACHSW = endpthi;
					facdata.asm_data->FACMSW = endptlo - 1;
				}
				fd = openWriteSaveFile (&write_save_file_state);
				if (fd > 0 &&
				    write_header (fd, FACTOR_MAGICNUM, FACTOR_VERSION, w) &&
				    write_long (fd, factor_found, NULL) &&
				    write_long (fd, bits, NULL) &&
				    write_long (fd, pass, NULL) &&
				    write_long (fd, facdata.asm_data->FACHSW, NULL) &&
				    write_long (fd, facdata.asm_data->FACMSW, NULL) &&
				    write_long (fd, endpthi, NULL) &&
				    write_long (fd, endptlo, NULL))
					closeWriteSaveFile (&write_save_file_state, fd);
				else {
					sprintf (buf, WRITEFILEERR, filename);
					OutputBoth (thread_num, buf);
					if (fd > 0) deleteWriteSaveFile (&write_save_file_state, fd);
				}
				if (stop_reason) {
					factorDone (&facdata);
					return (stop_reason);
				}
			}

/* Test for completion */

#ifdef X86_64
			if (facdata.pass_complete) goto nextpass;
#else
			if (facdata.asm_data->FACHSW > endpthi ||
			    (facdata.asm_data->FACHSW == endpthi && facdata.asm_data->FACMSW >= endptlo))
				goto nextpass;
#endif
		}

/* Set flag indicating a factor has been found! */

		factor_found = TRUE;

/* We used to continue factoring to find a smaller factor in a later pass. */
/* We'll continue to do this if the found factor is really small (less than */
/* 2^56) or if the user sets FindSmallestFactor in prime.ini. */

		find_smaller_factor = (end_bits <= (unsigned int) IniGetInt (INI_FILE, "FindSmallestFactor", 56));

/* Format and output a message for each found factor */

		do {
			makestr (facdata.asm_data->FACHSW, facdata.asm_data->FACMSW, facdata.asm_data->FACLSW, str);
			sprintf (buf, "M%ld has a factor: %s (TF:%d-%d)\n", p, str, (int) w->sieve_depth, (int) test_bits);
			OutputStr (thread_num, buf);
			formatMsgForResultsFile (buf, w);
			writeResults (buf);

/* Send assignment result to the server.  To avoid flooding the server */
/* with small factors from users needlessly redoing factoring work, make */
/* sure the factor is more than 60 bits or so. */

			if (strlen (str) >= 19 || IniGetInt (INI_FILE, "SendAllFactorData", 0)) {
				struct primenetAssignmentResult pkt;
				memset (&pkt, 0, sizeof (pkt));
				strcpy (pkt.computer_guid, COMPUTER_GUID);
				strcpy (pkt.assignment_uid, w->assignment_uid);
				strcpy (pkt.message, buf);
				pkt.result_type = PRIMENET_AR_TF_FACTOR;
				pkt.n = p;
				strcpy (pkt.factor, str);
				pkt.start_bits = (bits < report_bits) ? (unsigned int) w->sieve_depth : bits;
				pkt.done = !find_smaller_factor;
				spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
			}
		} while (getAnotherFactorIfAny (&facdata));

/* If we're looking for smaller factors, set a new end point.  Otherwise, */
/* skip all remaining passes. */

		if (!find_smaller_factor) break;

		if (facdata.asm_data->FACMSW != 0xFFFFFFFF) {
			endpthi = facdata.asm_data->FACHSW;
			endptlo = facdata.asm_data->FACMSW+1;
		} else {
			endpthi = facdata.asm_data->FACHSW+1;
			endptlo = 0;
		}
	        endpt = endpthi * 4294967296.0 + endptlo;

/* Do next of the 16 passes */

nextpass:	;
	    }

/* If we've found a factor then we need to send an assignment done */
/* message if we continued to look for a smaller factor. */

	    if (factor_found) {
		if (w->assignment_uid[0] && find_smaller_factor) {
			struct primenetAssignmentResult pkt;
			memset (&pkt, 0, sizeof (pkt));
			strcpy (pkt.computer_guid, COMPUTER_GUID);
			strcpy (pkt.assignment_uid, w->assignment_uid);
			pkt.result_type = PRIMENET_AR_NO_RESULT;
			pkt.done = TRUE;
			spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
		}
		break;
	    }

/* Output a no factor found message */

	    if (end_bits >= report_bits) {
		unsigned int start_bits;

		start_bits = (end_bits == report_bits) ?
				(unsigned int) w->sieve_depth : bits;
		if (start_bits < 32)
		    sprintf (buf,
			     "M%ld no factor to 2^%d, Wg%d: %08lX\n",
			     p, end_bits, PORT, SEC3 (p));
		else
		    sprintf (buf,
			     "M%ld no factor from 2^%d to 2^%d, Wg%d: %08lX\n",
			     p, start_bits, end_bits, PORT, SEC3 (p));
		OutputStr (thread_num, buf);
		formatMsgForResultsFile (buf, w);
		writeResults (buf);

/* Send no factor found message to the server for each bit */
/* level (i.e. one bit at a time).  As always to avoid swamping */
/* the server with needless data, do not send small bit level */
/* messages - that work has already been done. */

		if (end_bits >= 50 ||
		    IniGetInt (INI_FILE, "SendAllFactorData", 0)) {
			struct primenetAssignmentResult pkt;
			memset (&pkt, 0, sizeof (pkt));
			strcpy (pkt.computer_guid, COMPUTER_GUID);
			strcpy (pkt.assignment_uid, w->assignment_uid);
			strcpy (pkt.message, buf);
			pkt.result_type = PRIMENET_AR_TF_NOFACTOR;
			pkt.n = p;
			pkt.start_bits = start_bits;
			pkt.end_bits = end_bits;
			pkt.done = (w->work_type == WORK_FACTOR) &&
				   (end_bits >= test_bits);
			spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
		}
	    }

/* Advance the how far factored variable */

	    bits = end_bits;
	}

/* Clean up allocated factoring data */

	factorDone (&facdata);

/* Delete the continuation file(s) */

	unlinkSaveFiles (&write_save_file_state);

/* If we found a factor, then we likely performed much less work than */
/* we estimated.  Make sure we do not update the rolling average with */
/* this inaccurate data. */

	if (factor_found) invalidateNextRollingAverageUpdate ();

/* If we finished this work unit, return the happy news */

	if (factor_found || w->work_type == WORK_FACTOR)
		return (STOP_WORK_UNIT_COMPLETE);

/* Update the worktodo file */

	w->sieve_depth = bits;
	stop_reason = updateWorkToDoLine (thread_num, w);
	if (stop_reason) return (stop_reason);

/* All done */

	return (0);
}

/***************************************/
/* Routines to run a Lucas-Lehmer test */
/***************************************/

/* Structure for holding lucas setup data */

typedef struct {		/* Some of the data kept during LL test */
	gwhandle gwdata;	/* When we multithread the gwnum code, */
				/* gwsetup will return a handle */
	gwnum	lldata;		/* Number in the lucas sequence */
	unsigned long units_bit; /* Shift count */
} llhandle;

/* Prepare for running a Lucas-Lehmer test.  Caller must have already */
/* called gwinit. */

int lucasSetup (
	int	thread_num,	/* Worker thread number */
	unsigned long p,	/* Exponent to test */
	unsigned long fftlen,	/* Specific FFT length to use, or zero */
	llhandle *lldata)	/* Common LL data structure */
{
	int	res;
	int	c;

/* Init LL data structure */

	lldata->lldata = NULL;
	lldata->units_bit = 0;

/* As a kludge for the benchmarking and timing code, an odd FFTlen sets up gwnum for all-complex FFTs. */

	c = (fftlen & 1) ? 1 : -1;
	fftlen &= ~1;

/* Init the FFT code for squaring modulo 2^p+c */

	gwset_safety_margin (&lldata->gwdata, IniGetFloat (INI_FILE, "ExtraSafetyMargin", 0.0));
	gwset_minimum_fftlen (&lldata->gwdata, fftlen);
	if (!IniGetInt (INI_FILE, "GW_BENCH_SPECIAL", 0))
		res = gwsetup (&lldata->gwdata, 1.0, 2, p, c);
	else {	/* Special INI settings for my use only to benchmark any k,b,n,c value */
		res = gwsetup (&lldata->gwdata, 
			       IniGetFloat (INI_FILE, "GW_BENCH_SPECIAL_K", 1),
			       IniGetInt (INI_FILE, "GW_BENCH_SPECIAL_B", 2),
			       IniGetInt (INI_FILE, "GW_BENCH_SPECIAL_N", p),
			       IniGetInt (INI_FILE, "GW_BENCH_SPECIAL_C", -1));
	}

/* If we were unable to init the FFT code, then print an error message */
/* and return an error code.  There is one exception, when we are doing */
/* a benchmark of all possible FFT implementations, do not print an error */
/* message. */

	if (res) {
		if (!lldata->gwdata.bench_pick_nth_fft) {
			char	buf[180];
			sprintf (buf, "Cannot initialize FFT code, errcode=%d\n", res);
			OutputBoth (thread_num, buf);
			gwerror_text (&lldata->gwdata, res, buf, sizeof (buf) - 1);
			strcat (buf, "\n");
			OutputBoth (thread_num, buf);
		}
		return (STOP_FATAL_ERROR);
	}

/* Allocate memory for the Lucas-Lehmer data (the number to square) */

	lldata->lldata = gwalloc (&lldata->gwdata);
	if (lldata->lldata == NULL) {
		gwdone (&lldata->gwdata);
		OutputStr (thread_num, "Error allocating memory for FFT data.\n");
		return (STOP_OUT_OF_MEM);
	}
	return (0);
}

/* Clean up after running a Lucas-Lehmer test */

void lucasDone (
	llhandle *lldata)	/* Common LL data structure */
{

/* Free memory for the Lucas-Lehmer data */

	gwfree (&lldata->gwdata, lldata->lldata);

/* Cleanup the FFT code */

	gwdone (&lldata->gwdata);
}

/* Generate the 64-bit residue of a Lucas-Lehmer test.  Returns -1 for an */
/* illegal result, 0 for a zero result, 1 for a non-zero result. */

int generateResidue64 (
	llhandle *lldata,
	unsigned long p,	/* Exponent under LL test */
	unsigned long *reshi,
	unsigned long *reslo)
{
	uint64_t res64;
	giant	tmp;
	int	err_code;

	*reshi = *reslo = 0;
	tmp = popg (&lldata->gwdata.gdata, (p >> 5) + 5);
	err_code = gwtogiant (&lldata->gwdata, lldata->lldata, tmp);
	if (err_code < 0) {
		pushg (&lldata->gwdata.gdata, 1);
		return (err_code);
	}
	if (tmp->sign == 0) {
		pushg (&lldata->gwdata.gdata, 1);
		return (0);
	}

	/* If the shift count is large, we will need some of the bottom bits too */
	res64 = 0;
	if (lldata->units_bit + 64 > p) {
		if (tmp->sign > 0) res64 = tmp->n[0];
		if (tmp->sign > 1) res64 += ((uint64_t) tmp->n[1]) << 32;
		res64 <<= p - lldata->units_bit;
		if (p < 64) res64 &= (((uint64_t) 1) << p) - 1;
	}

	/* Apply the shift count and get the low 64 bits */
	gshiftright (lldata->units_bit, tmp);
	if (tmp->sign > 0) res64 += tmp->n[0];
	if (tmp->sign > 1) res64 += ((uint64_t) tmp->n[1]) << 32;
	pushg (&lldata->gwdata.gdata, 1);

	/* Return the calculated 64-bit residue */
	*reslo = (uint32_t) res64;
	*reshi = (uint32_t) (res64 >> 32);
	return (1);
}

/* Write intermediate Lucas-Lehmer results to a file */
/* The LL save file format is: */
/*	52-bytes	standard header for all work types */
/*	u32		error_count */
/*	u32		iteration counter */
/*	u32		shift_count */
/*	gwnum		FFT data (u32 len, array u32s) */

#define LL_MAGICNUM		0x2c7330a8
#define LL_VERSION		1
#define LL_ERROR_COUNT_OFFSET	52

int writeLLSaveFile (
	llhandle *lldata,
	writeSaveFileState *write_save_file_state,
	struct work_unit *w,
	unsigned long counter,
	unsigned long error_count)
{
	int	fd;
	unsigned long sum = 0;

/* Open the save file */

	fd = openWriteSaveFile (write_save_file_state);
	if (fd < 0) return (FALSE);

	if (!write_header (fd, LL_MAGICNUM, LL_VERSION, w)) goto err;

	if (!write_long (fd, error_count, &sum)) goto err;
	if (!write_long (fd, counter, &sum)) goto err;
	if (!write_long (fd, lldata->units_bit, &sum)) goto err;
	if (!write_gwnum (fd, &lldata->gwdata, lldata->lldata, &sum)) goto err;

	if (!write_checksum (fd, sum)) goto err;

	closeWriteSaveFile (write_save_file_state, fd);
	return (TRUE);

/* An error occured.  Delete the current file. */

err:	deleteWriteSaveFile (write_save_file_state, fd);
	return (FALSE);
}

/* Update the error count in an intermediate file */

void writeNewErrorCount (
	char	*filename,
	unsigned long new_error_count)
{
	int	fd;
	unsigned long sum, old_error_count;

/* Open the intermediate file, position past the FFT data */

	fd = _open (filename, _O_BINARY | _O_RDWR);
	if (fd < 0) return;

/* Read in the checksum and old error count */

	if (!read_checksum (fd, &sum)) goto err;
	_lseek (fd, LL_ERROR_COUNT_OFFSET, SEEK_SET);
	if (!read_long (fd, &old_error_count, NULL)) goto err;

/* Update the checksum */

	sum = sum - old_error_count + new_error_count;

/* Write out the checksum and new error count */

	if (!write_checksum (fd, sum)) goto err;
	_lseek (fd, LL_ERROR_COUNT_OFFSET, SEEK_SET);
	if (!write_long (fd, new_error_count, NULL)) goto err;

/* Close file and return */

err:	_close (fd);
}

/* Read the data portion of an intermediate Lucas-Lehmer results file */

int readLLSaveFile (
	llhandle *lldata,
	char	*filename,
	struct work_unit *w,
	unsigned long *counter,
	unsigned long *error_count)
{
	int	fd;
	unsigned long sum, filesum, version;

	fd = _open (filename, _O_BINARY | _O_RDONLY);
	if (fd <= 0) return (FALSE);

	if (!read_magicnum (fd, LL_MAGICNUM)) goto err;
	if (!read_header (fd, &version, w, &filesum)) goto err;
	if (version != LL_VERSION) goto err;

	sum = 0;
	if (!read_long (fd, error_count, &sum)) goto err;
	if (!read_long (fd, counter, &sum)) goto err;
	if (!read_long (fd, &lldata->units_bit, &sum)) goto err;
	if (lldata->units_bit >= lldata->gwdata.n) goto err;

	if (!read_gwnum (fd, &lldata->gwdata, lldata->lldata, &sum)) goto err;

	if (filesum != sum) goto err;
	_close (fd);
	return (TRUE);
err:	_close (fd);
	return (FALSE);
}

/* Increment the error counter.  The error counter is one 32-bit field containing 5 values.  Prior to version 29.3, this was */
/* a one-bit flag if this is a continuation from a save file that did not track error counts, a 7-bit count of errors that were */
/* reproducible, a 8-bit count of ILLEGAL SUMOUTs or zeroed FFT data or corrupt units_bit, a 8-bit count of convolution errors */
/* above 0.4, and a 8-bit count of SUMOUTs not close enough to SUMINPs. */
/* NOTE:  The server considers an LL run clean if the error code is XXaaYY00 and XX = YY and aa is ignored.  That is, repeatable */
/* round off errors and all ILLEGAL SUMOUTS are ignored. */
/* In version 29.3, a.k.a. Wf in result lines, the 32-bit field changed.  See comments in the code below. */

void inc_error_count (
	int	type,
	unsigned long *error_count)
{
	unsigned long addin, orin, maxval;

	addin = orin = 0;
	if (type == 0) addin = 1, maxval = 0xF;				// SUMINP != SUMOUT
	else if (type == 4) addin = 1 << 4, maxval = 0x0F << 4;		// Jacobi error check
	else if (type == 1) addin = 1 << 8, maxval = 0x3F << 8;		// Roundoff > 0.4
	else if (type == 5) orin = 1 << 14;				// Zeroed FFT data
	else if (type == 6) orin = 1 << 15;				// Units bit counter corrupted
	else if (type == 2) addin = 1 << 16, maxval = 0xF << 16;	// ILLEGAL SUMOUT
	else if (type == 7) addin = 1 << 20, maxval = 0xF << 20;	// High reliability (Gerbicz or dblchk) PRP error
	else if (type == 3) addin = 1 << 24, maxval = 0x3F << 24;	// Repeatable error

	if (addin && (*error_count & maxval) != maxval) *error_count += addin;
	*error_count |= orin;
}

/* Create a message if the non-repeatable error count is more than zero */
/* Returns TRUE if the non-repeatable error count is more than zero. */

int make_error_count_message (
	unsigned long error_count,
	int	message_type,		/* 1 = very small, 2 = one line, 3 = multi-line, 0x8000 = confidence always excellent */
	char	*buf,
	int	buflen)
{
	int	count_repeatable, count_suminp, count_roundoff, count_illegal_sumout, count_total;
	int	count_jacobi, count_gerbicz, count_bad_errors;
	int	force_high_confidence;
	char	local_buf[400], counts_buf[200], confidence[25];

/* Massage input argument */

	force_high_confidence = message_type & 0x8000;
	message_type = message_type & 0x7FFF;

/* Parse the error counts variable */

	count_repeatable = (error_count >> 24) & 0x3F;
	count_illegal_sumout = (error_count >> 16) & 0xF;
	count_roundoff = (error_count >> 8) & 0x3F;
	count_jacobi = (error_count >> 4) & 0xF;
	count_gerbicz = (error_count >> 20) & 0xF;
	count_suminp = error_count & 0xF;

/* Return if no hardware errors have occurred */

	count_total = count_illegal_sumout + count_suminp + count_roundoff + count_jacobi + count_gerbicz;
	if (count_total - count_repeatable == 0) return (FALSE);

/* Format the error counts */

	counts_buf[0] = 0;

	if (message_type == 1) {
		sprintf (counts_buf, ", errors: %d", count_total);
	}

	if (message_type == 2) {
		if (count_total == 1)
			strcpy (counts_buf, "1 error");
		else
			sprintf (counts_buf, "%d errors", count_total);
		if (count_repeatable >= 1)
			sprintf (local_buf, ", %d were repeatable (not errors)", count_repeatable);
	}

	if (message_type == 3) {
		if (count_jacobi >= 1) {
			sprintf (local_buf, "%d Jacobi errors, ", count_jacobi);
			strcat (counts_buf, local_buf);
		}
		if (count_gerbicz >= 1) {
			sprintf (local_buf, "%d Gerbicz/double-check errors, ", count_gerbicz);
			strcat (counts_buf, local_buf);
		}
		if (count_roundoff >= 1) {
			sprintf (local_buf, "%d ROUNDOFF > 0.4, ", count_roundoff);
			strcat (counts_buf, local_buf);
		}
		if (count_suminp >= 1) {
			sprintf (local_buf, "%d SUM(INPUTS) != SUM(OUTPUTS), ", count_suminp);
			strcat (counts_buf, local_buf);
		}
		if (count_illegal_sumout >= 1) {
			sprintf (local_buf, "%d ILLEGAL SUMOUT, ", count_illegal_sumout);
			strcat (counts_buf, local_buf);
		}
		counts_buf[strlen(counts_buf)-2] = 0;
		if (count_repeatable >= 1) {
			sprintf (local_buf, "of which %d were repeatable (not hardware errors)", count_repeatable);
			if (strlen (counts_buf) <= 40) strcat (counts_buf, " ");
			else strcat (counts_buf, "\n");
			strcat (counts_buf, local_buf);
		}
		strcat (counts_buf, ".\n");
	}

/* Guess our confidence in the end result */

	count_bad_errors = count_jacobi + count_suminp + count_roundoff - count_repeatable;
	if (force_high_confidence) count_bad_errors = 0;
	strcpy (confidence, count_bad_errors == 0 ? "excellent" :
			    count_bad_errors <= 3 ? "fair" :
			    count_bad_errors <= 6 ? "poor" : "very poor");

/* Put it all together to form our full message */

	if (message_type == 1) {
		sprintf (local_buf, ", %s, confidence: %s", counts_buf, confidence);
	}
	if (message_type == 2) {
		if (count_jacobi || count_gerbicz) sprintf (local_buf, "Hardware errors!  %s.  Confidence in end result is %s.\n", counts_buf, confidence);
		else sprintf (local_buf, "Possible hardware errors!  %s.  Confidence in end result is %s.\n", counts_buf, confidence);
	}
	if (message_type == 3) {
		if (count_jacobi || count_gerbicz) strcpy (local_buf, "Hardware errors have occurred during the test!");
		else strcpy (local_buf, "Possible hardware errors have occurred during the test!");
		if (strlen (counts_buf) <= 25) strcat (local_buf, " ");
		else strcat (local_buf, "\n");
		strcat (local_buf, counts_buf);
		sprintf (local_buf+strlen(local_buf), "Confidence in final result is %s.\n", confidence);
	}

/* Copy as much of our result as possible to the caller's buffer */

	if ((int) strlen (local_buf) >= buflen) local_buf[buflen-1] = 0;
	strcpy (buf, local_buf);
	return (TRUE);
}


/* Prepare for subtracting 2 from the squared result.  Also keep track */
/* of the location of the ever changing units bit. */

void lucas_fixup (
	llhandle *lldata,
	unsigned long p)	/* Exponent being tested */
{

/* We are about to square the number, the units bit position will double */

	lldata->units_bit <<= 1;
	if (lldata->units_bit >= p) lldata->units_bit -= p;

/* Tell gwnum code the value to subtract 2 from the squared result. */

	gwsetaddinatpowerofb (&lldata->gwdata, -2, lldata->units_bit);
}

/* Generate random FFT data for timing the Lucas-Lehmer code */

void generateRandomData (
	llhandle *lldata)
{
	unsigned long i;

/* Fill data space with random values. */

	srand ((unsigned) time (NULL));
	for (i = 0; i < gwfftlen (&lldata->gwdata); i++) {
		set_fft_value (&lldata->gwdata, lldata->lldata, i, rand() & 0xFF);
	}
}

/* For exponents that are near an FFT limit, do 1000 sample iterations */
/* to see if we should use the smaller or larger FFT size.  We examine */
/* the average roundoff error to determine which FFT size to use. */

int pick_fft_size (
	int	thread_num,
	struct work_unit *w)
{
	llhandle lldata;
	char	buf[120];
	double	softpct, total_error, avg_error, max_avg_error;
	unsigned long small_fftlen, large_fftlen;
	int	i, stop_reason;

/* We only do this for Mersenne numbers */

	if (w->k != 1.0 || w->b != 2 || w->c != -1) return (0);

/* We don't do this for small exponents.  We've not studied the average */
/* error enough on smaller FFT sizes to intelligently pick the FFT size. */
/* Also, for really large exponents there is no larger FFT size to use! */

	if (w->n <= 5000000) return (0);

/* If we've already calculated the best FFT size, then return */

	if (w->minimum_fftlen) return (0);

/* Get info on what percentage of exponents on either side of */
/* an FFT crossover we will do this 1000 iteration test. */

	softpct = IniGetFloat (INI_FILE, "SoftCrossover", (float) 0.2) / 100.0;

/* If this exponent is not close to an FFT crossover, then we are done */

	small_fftlen = gwmap_to_fftlen (1.0, 2, (unsigned long) ((1.0 - softpct) * w->n), -1);
	large_fftlen = gwmap_to_fftlen (1.0, 2, (unsigned long) ((1.0 + softpct) * w->n), -1);
	if (small_fftlen == large_fftlen || large_fftlen == 0) return (0);

/* Let the user be more conservative or more aggressive in picking the */
/* acceptable average error.  By default, we accept an average error */
/* between 0.241 and 0.243 depending on the FFT size. */

	max_avg_error = 0.241 + 0.002 *
		(log ((double) small_fftlen) - log ((double) 262144.0)) /
		(log ((double) 4194304.0) - log ((double) 262144.0));
	max_avg_error += IniGetFloat (INI_FILE, "SoftCrossoverAdjust", 0.0);

/* Print message to let user know what is going on */

	sprintf (buf,
		 "Trying 1000 iterations for exponent %ld using %luK FFT.\n",
		 w->n, small_fftlen / 1024);
	OutputBoth (thread_num, buf);
	sprintf (buf,
		 "If average roundoff error is above %.5g, then a larger FFT will be used.\n",
		 max_avg_error);
	OutputBoth (thread_num, buf);

/* Init the FFT code using the smaller FFT size */

	gwinit (&lldata.gwdata);
	gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
	stop_reason = lucasSetup (thread_num, w->n, small_fftlen, &lldata);
	if (stop_reason) return (stop_reason);

/* Fill data space with random values then do one squaring to make */
/* the data truly random. */

	generateRandomData (&lldata);
	gwsetnormroutine (&lldata.gwdata, 0, TRUE, 0);
	gwstartnextfft (&lldata.gwdata, TRUE);
	gwsquare (&lldata.gwdata, lldata.lldata);

/* Average the roundoff error over a 1000 iterations. */

	for (i = 0, total_error = 0.0; ; ) {
		gw_clear_maxerr (&lldata.gwdata);
		gwsquare (&lldata.gwdata, lldata.lldata);
		total_error += gw_get_maxerr (&lldata.gwdata);
		stop_reason = stopCheck (thread_num);
		if (stop_reason) {
			lucasDone (&lldata);
			return (stop_reason);
		}
		if (++i == 1000) break;
		if (i % 100 == 0) {
			sprintf (buf,
				 "After %d iterations average roundoff error is %.5g.\n",
				 i, total_error / (double) i);
			OutputStr (thread_num, buf);
		}
	}
	avg_error = total_error / 1000.0;
	lucasDone (&lldata);

/* Now decide which FFT size to use based on the average error. */
/* Save this info in worktodo.ini so that we don't need to do this again. */

	w->minimum_fftlen = (avg_error <= max_avg_error) ? small_fftlen : large_fftlen;
	stop_reason = updateWorkToDoLine (thread_num, w);
	if (stop_reason) return (stop_reason);

/* Output message to user informing him of the outcome. */

	sprintf (buf,
		 "Final average roundoff error is %.5g, using %luK FFT for exponent %ld.\n",
		 avg_error, w->minimum_fftlen / 1024, w->n);
	OutputBoth (thread_num, buf);
	return (0);
}

/* Test if we are near the maximum exponent this fft length can test */
/* We only support this (careful iterations when near fft limit) for */
/* Mersenne numbers. */

int exponent_near_fft_limit (
	gwhandle *gwdata)		/* Handle returned by gwsetup */
{
	return (gwnear_fft_limit (gwdata, IniGetFloat (INI_FILE, "NearFFTLimitPct", 0.5)));
}

/* Output the good news of a new prime to the screen in an infinite loop */

void good_news (void *arg)
{
	char	buf[80];

	title (MAIN_THREAD_NUM, "New Prime!!!");
	sprintf (buf, "New Mersenne Prime!!!!  M%d is prime!\n", (int) (intptr_t) arg);
	while (WORKER_THREADS_ACTIVE && ! WORKER_THREADS_STOPPING) {
		OutputStr (MAIN_THREAD_NUM, buf);
		flashWindowAndBeep ();
		Sleep (50);
	}
}

/* Rotate a p-bit giant right shift_count bits.  Used to undo shifted FFT data. */

int rotateg (				/* Return false if failed due to memory allocation error */
	giant	v,			/* Giant to rotate right */
	unsigned long p,		/* Mersenne exponent (bit size of the giant to rotate) */
	unsigned long shift_count,	/* Number of bits to rotate right */
	ghandle	*gdata)			/* Handle for allocating giant temporaries */
{
	giant	vlo;

/* If rotate count is zero, no work needed */

	if (shift_count == 0) return (1);

/* Convert current iteration to binary */

	vlo = popg (gdata, (p >> 5) + 5);
	if (vlo == NULL) return (0);

/* Apply the shift count */

	gtogshiftright (shift_count, v, vlo);		// Shift right and copy
	gmaskbits (shift_count, v);			// Mask for the bits below the shift count
	gshiftleft (p - shift_count, v);		// Shift the masked bits left
	addg (vlo, v);					// Recombine the bits, now rotated
	pushg (gdata, 1);				// Free the temporary
	return (1);
}

/* Perform a Jacobi test on the current LL iteration.  This check has a 50% chance of catching */
/* a calculation error.  See http://www.mersenneforum.org/showthread.php?t=22471 especially */
/* starting at post #30. */

int jacobi_test (
	int	thread_num,		/* Window to display messages in */
	unsigned long p,		/* Mersenne exponent */
	llhandle *lldata)		/* Struct that points us to the LL data */
{
	giant	v;
	mpz_t	a, b;
	int	err_code, Jacobi_symbol, silent_Jacobi;
	double	timers[1];
	char	buf[80];

/* Clear and start a timer */

	clear_timers (timers, sizeof (timers) / sizeof (timers[0]));
	start_timer (timers, 0);

/* Convert current iteration to binary.  Apply the shift count. */

	v = popg (&lldata->gwdata.gdata, (p >> 5) + 5);
	if (v == NULL) goto oom;
	err_code = gwtogiant (&lldata->gwdata, lldata->lldata, v);
	if (err_code < 0) {		/* LL value could not be calculated.  Should not happen, return failed-Jacobi-test */
		OutputBoth (thread_num, "LL value corrupt.  Could not run Jacobi error check.\n");
		pushg (&lldata->gwdata.gdata, 1);
		return (0);
	}
	if (! rotateg (v, p, lldata->units_bit, &lldata->gwdata.gdata)) {
		pushg (&lldata->gwdata.gdata, 1);
		goto oom;
	}

/* Copy the LL value to "a" */

	mpz_init (a);
	gtompz (v, a);
	pushg (&lldata->gwdata.gdata, 1);

/* Generate the Mersenne number */

	mpz_init (b);
	mpz_ui_pow_ui (b, 2, p);
	mpz_sub_ui (b, b, 1);

/* Free giants, compute the Jacobi symbol (a-2|Mp) */

	silent_Jacobi = IniGetInt (INI_FILE, "SilentJacobi", 0);
	if (!silent_Jacobi) OutputStr (thread_num, "Running Jacobi error check.  ");
	mpz_sub_ui (a, a, 2);
	if (mpz_sgn (a) < 0) mpz_add (a, a, b);
	Jacobi_symbol = mpz_jacobi (a, b);

/* End the timer, print out PASS/FAIL message along with time taken */

	end_timer (timers, 0);
	sprintf (buf, "%s.  Time: %6.3f sec.\n", Jacobi_symbol == -1 ? "Passed" : "Failed", timer_value (timers, 0));
	if (!silent_Jacobi) OutputStrNoTimeStamp (thread_num, buf);
	else if (Jacobi_symbol != -1) OutputStr (thread_num, "Jacobi error-check failed\n");

/* Cleanup and return */

	mpz_clear (a);
	mpz_clear (b);
	return (Jacobi_symbol == -1);

/* Memory allocation error */

oom:	OutputBoth (thread_num, "Memory allocation error.  Could not run Jacobi error check.\n");
	return (1);			/* Assume the Jacobi test would have passed */
}

/* Do the Lucas-Lehmer test */

int prime (
	int	thread_num,		/* Worker thread number */
	struct PriorityInfo *sp_info,	/* SetPriority information */
	struct work_unit *w,		/* Worktodo entry */
	int	pass)			/* PrimeContinue pass */
{
	llhandle lldata;
	unsigned long p;
	unsigned long counter;
	unsigned long error_count;
	unsigned long restart_error_count = 0;	/* On a restart, use this error count rather than the one from a save file */
	unsigned long iters;
	readSaveFileState read_save_file_state; /* Manage savefile names during reading */
	writeSaveFileState write_save_file_state; /* Manage savefile names during writing */
	char	filename[32];
	double	timers[2];
	double	inverse_p;
	double	reallyminerr = 1.0;
	double	reallymaxerr = 0.0;
	double	*addr1;
	int	Jacobi_testing_enabled;
	int	first_iter_msg, near_fft_limit, sleep5;
	unsigned long high32, low32;
	int	rc, isPrime, stop_reason;
	char	buf[400], fft_desc[100];
	int	slow_iteration_count;
	double	best_iteration_time;
	unsigned long last_counter = 0xFFFFFFFF;	/* Iteration of last error */
	int	maxerr_recovery_mode = 0;		/* Big roundoff err rerun */
	double	last_suminp = 0.0;
	double	last_sumout = 0.0;
	double	last_maxerr = 0.0;
	double	allowable_maxerr, output_frequency, output_title_frequency;
	int	error_count_messages;

/* Initialize */

	p = w->n;

/* Grab setting from INI file.  Fast (sub-quadratic) Jacobi testing was introduced in GMP version 5.1.0. */

	Jacobi_testing_enabled = IniGetInt (INI_FILE, "JacobiErrorCheck", 1);
	{
		int	major_ver, minor_ver;
		sscanf (gmp_version, "%d.%d", &major_ver, &minor_ver);
		if (major_ver < 5 || (major_ver == 5 && minor_ver < 1)) Jacobi_testing_enabled = FALSE;
	}

/* Do some of the trial factoring.  We treat factoring that is part of a */
/* LL test as priority work (done in pass 1).  We don't do all the trial */
/* factoring as the last bit level takes a lot of time and is unlikely */
/* to find a factor.  The P-1 test will probably be needed anyway and */
/* may find a factor thus saving us from doing the last bit level. */

	if ((w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK) &&
	    ! IniGetInt (INI_FILE, "SkipTrialFactoring", 0)) {
		struct PriorityInfo sp_info_copy;
		memcpy (&sp_info_copy, sp_info, sizeof (struct PriorityInfo));
		stop_reason = primeFactor (thread_num, &sp_info_copy, w, 1);
		if (stop_reason) return (stop_reason);
	}

/* See if this exponent needs P-1 factoring.  We treat P-1 factoring */
/* that is part of an LL test as priority work done in pass 1 or as */
/* regular work done in pass 2 if WellBehavedWork or SequentialWorkTodo */
/* is set.  The only way we can get to pass 3 and P-1 still needs to be */
/* done is if pfactor returned STOP_NOT_ENOUGH_MEM on an earlier pass. */
/* In that case, skip onto doing the LL test until more memory becomes */
/* available. */

	if ((w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK) &&
	    ! w->pminus1ed && pass != 3) {
		int	pass_to_pfactor;

		pass_to_pfactor = (WELL_BEHAVED_WORK || SEQUENTIAL_WORK) ? 2 : 1;
		if (pass != pass_to_pfactor) return (0);

		stop_reason = pfactor (thread_num, sp_info, w);
		if (stop_reason) return (stop_reason);
	}

/* Do the rest of the trial factoring. */

	if ((w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK) &&
	    ! IniGetInt (INI_FILE, "SkipTrialFactoring", 0)) {
		struct PriorityInfo sp_info_copy;
		memcpy (&sp_info_copy, sp_info, sizeof (struct PriorityInfo));
		stop_reason = primeFactor (thread_num, &sp_info_copy, w, 0);
		if (stop_reason) return (stop_reason);
	}

/* Done with pass 1 priority work.  Return to do more priority work. */

	if (pass == 1 && w->work_type != WORK_ADVANCEDTEST) return (0);

/* Figure out which FFT size we should use */

	stop_reason = pick_fft_size (thread_num, w);
	if (stop_reason) return (stop_reason);

/* Make sure the first-time user runs a successful self-test. */
/* The one-hour self-test may have been useful when it was first introduced */
/* but I think it now does little to catch buggy machines (they eventually */
/* work OK for an hour) and does create user confusion and annoyance. */

#ifdef ONE_HOUR_SELF_TEST
	if (w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK) {
		stop_reason = selfTest (thread_num, sp_info, w);
		if (stop_reason) return (stop_reason);
	}
#endif

/* Setup the LL test */

begin:	gwinit (&lldata.gwdata);
	if (IniGetInt (LOCALINI_FILE, "UseLargePages", 0))
		gwset_use_large_pages (&lldata.gwdata);
	gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
	if (HYPERTHREAD_LL) {
		sp_info->normal_work_hyperthreads = IniGetInt (LOCALINI_FILE, "HyperthreadLLcount", CPU_HYPERTHREADS);
		gwset_will_hyperthread (&lldata.gwdata, sp_info->normal_work_hyperthreads);
	}
	gwset_bench_cores (&lldata.gwdata, NUM_CPUS);
	gwset_bench_workers (&lldata.gwdata, NUM_WORKER_THREADS);
	if (ERRCHK) gwset_will_error_check (&lldata.gwdata);
	else gwset_will_error_check_near_limit (&lldata.gwdata);
	gwset_num_threads (&lldata.gwdata, CORES_PER_TEST[thread_num] * sp_info->normal_work_hyperthreads);
	gwset_thread_callback (&lldata.gwdata, SetAuxThreadPriority);
	gwset_thread_callback_data (&lldata.gwdata, sp_info);
	stop_reason = lucasSetup (thread_num, p, w->minimum_fftlen, &lldata);
	if (stop_reason) return (stop_reason);

/* Record the amount of memory being used by this thread. */

	set_memory_usage (thread_num, 0, cvt_gwnums_to_mem (&lldata.gwdata, 1));

/* Loop reading from save files (and backup save files).  Limit number of backup */
/* files we try to read in case there is an error deleting bad save files. */

	tempFileName (w, filename);
	readSaveFileStateInit (&read_save_file_state, thread_num, filename);
	writeSaveFileStateInit (&write_save_file_state, filename, NUM_JACOBI_BACKUP_FILES);
	for ( ; ; ) {

/* If there are no more save files, start off with the 1st Lucas number. */

		if (! saveFileExists (&read_save_file_state)) {
			/* If there were save files, they are all bad.  Report a message */
			/* and temporarily abandon the work unit.  We do this in hopes that */
			/* we can successfully read one of the bad save files at a later time. */
			/* This sounds crazy, but has happened when OSes get in a funky state. */
			if (read_save_file_state.a_non_bad_save_file_existed ||
			    (pass == 3 && read_save_file_state.a_save_file_existed)) {
				OutputBoth (thread_num, ALLSAVEBAD_MSG);
				return (0);
			}
			/* No save files existed, start from scratch. */
			counter = 2;
			error_count = 0;
			first_iter_msg = FALSE;
			break;
		}

/* Read an LL save file.  If successful, break out of loop. */

		if (readLLSaveFile (&lldata, read_save_file_state.current_filename, w, &counter, &error_count) &&
		    counter <= w->n &&
		    (!Jacobi_testing_enabled || jacobi_test (thread_num, p, &lldata))) {
			first_iter_msg = TRUE;
			if (Jacobi_testing_enabled) write_save_file_state.special = 1;
			break;
		}

/* On read error, output message and loop to try the next backup save file. */

		saveFileBad (&read_save_file_state);
	}

/* If this is a restart from an error, use the incremented error_count in restart_error_count */
/* rather than the error_count from a save file. */

	if (restart_error_count) error_count = restart_error_count;

/* Hyperthreading backoff is an option to pause the program when iterations */
/* take longer than usual.  This is useful on hyperthreaded machines so */
/* that prime95 doesn't steal cycles from a foreground task, thus hurting */
/* the computers responsiveness. */

	best_iteration_time = 1.0e50;
	slow_iteration_count = 0;

/* Clear all timers */

	clear_timers (timers, sizeof (timers) / sizeof (timers[0]));

/* Init the title */

	sprintf (buf, "%ld / %ld", counter, p);
	title (thread_num, buf);

/* Init vars for Test/Status and CommunicateWithServer */

	strcpy (w->stage, "LL");
	inverse_p = 1.0 / (double) p;
	w->pct_complete = (double) counter * inverse_p;
	calc_output_frequencies (&lldata.gwdata, &output_frequency, &output_title_frequency);

/* Start off with the 1st Lucas number - four (ATH requested making the starting value overriddable). */
/* Note we do something a little strange here.  We actually set the first number to 4 but shifted by */
/* a random amount.  This lets two different machines check the same Mersenne number and operate on */
/* different FFT data - thus greatly reducing the chance that a CPU or program error corrupts the results. */

	if (counter == 2) {
		unsigned long S0;
		if ((S0 = IniGetInt (INI_FILE, "InitialLLValue", 4)) != 4) {
			if (S0 == 23) {	/* 23 is not a valid start value. Use 23 as a secret code for 2/3.  Courtesy of Batalov. */
				giant tmp;
				tmp = allocgiant ((p >> 5) + 5);
				if (tmp == NULL) return (OutOfMemory (thread_num));
				ultog (2, tmp);
				power (tmp, p);
				iaddg (1, tmp);
				dbldivg (3, tmp);
				gianttogw (&lldata.gwdata, tmp, lldata.lldata);
			} else {
				dbltogw (&lldata.gwdata, (double) S0, lldata.lldata);
			}
			lldata.units_bit = 0;
		} else {
			unsigned long i, word, bit_in_word;
			uint32_t hi, lo;
			// Generate a random initial shift count
			srand ((unsigned) time (NULL));
			lldata.units_bit = (rand () << 16) + rand ();
			if (CPU_FLAGS & CPU_RDTSC) { rdtsc(&hi,&lo); lldata.units_bit += lo; }
			// Let user override random initial shift count
			lldata.units_bit = IniGetInt (INI_FILE, "InitialShiftCount", lldata.units_bit);
			// Initial shift count can't be larger than p
			lldata.units_bit = lldata.units_bit % p;
			// Perform the initial shift
			bitaddr (&lldata.gwdata, (lldata.units_bit + 2) % p, &word, &bit_in_word);
			for (i = 0; i < gwfftlen (&lldata.gwdata); i++) {
				set_fft_value (&lldata.gwdata, lldata.lldata, i, (i == word) ? (1L << bit_in_word) : 0);
			}
		}
	}

/* Output a message indicating we are starting/resuming an LL test. */
/* Also tell user the FFT length. */

	gwfft_description (&lldata.gwdata, fft_desc);
	sprintf (buf, "%s primality test of M%ld using %s\n", (counter == 2) ? "Starting" : "Resuming", p, fft_desc);
	OutputStr (thread_num, buf);

/* If we are near the maximum exponent this fft length can test, then we */
/* will error check all iterations */

	near_fft_limit = exponent_near_fft_limit (&lldata.gwdata);

/* Figure out the maximum round-off error we will allow.  By default this is 27/64 when near the FFT limit and 26/64 otherwise. */
/* We've found that this default catches errors without raising too many spurious error messages.  We let the user override */
/* this default for user "Never Odd Or Even" who tests exponents well beyond an FFT's limit.  He does his error checking by */
/* running the first-test and double-check simultaneously. */

	allowable_maxerr = IniGetFloat (INI_FILE, "MaxRoundoffError", (float) (near_fft_limit ? 0.421875 : 0.40625));

/* Get address of second FFT data element.  We'll use this for very */
/* quickly checking for zeroed FFT data. */

	addr1 = addr (&lldata.gwdata, lldata.lldata, 1);

/* Compute numbers in the lucas series, write out every 30 minutes to a file */

	iters = 0;
	error_count_messages = IniGetInt (INI_FILE, "ErrorCountMessages", 3);
	while (counter < p) {
		int	saving, Jacobi_testing, echk, sending_residue, interim_residue, interim_file;
		int	actual_frequency;

/* See if we should stop processing after this iteration */

		stop_reason = stopCheck (thread_num);

/* Save if we are stopping, right after we pass an errored iteration, several iterations before retesting */
/* an errored iteration so that we don't have to backtrack very far to do a gwsquare_carefully iteration */
/* (we don't do the iteration immediately before because a save operation may change the FFT data and make */
/* the error non-reproducible), if Jacobi testing, and finally save if the save file timer has gone off. */

		saving = counter+1 != p &&
			 (stop_reason || counter == last_counter-8 || counter == last_counter ||
			  Jacobi_testing || testSaveFilesFlag (thread_num));

/* Run a Jacobi test on the last iteration and the first iteration after the Jacobi timer goes off that we */
/* happen to be creating a save file.  This works around the minor issue where the save file timer and Jacobi timer */
/* go off close to one another generating two save files just a few iterations apart. */

		Jacobi_testing = Jacobi_testing_enabled && (counter+1 == p || (!stop_reason && saving && testJacobiFlag (thread_num)));

/* Error check before writing an intermediate file, if near an FFT's limit, if user requested it, */
/* the last 50 iterations, and every 128th iteration. */

		echk = saving || near_fft_limit || ERRCHK || (counter >= p - 50) || ((counter & 127) == 0);
		gw_clear_maxerr (&lldata.gwdata);

/* Check if we should send residue to server, output residue to screen, or create an interediate save file */

		sending_residue = ((counter+1) == 500002 || ((counter+1) % 5000000 == 2 && IniGetInt (INI_FILE, "SendInterimResidues", 1)));
		interim_residue = (INTERIM_RESIDUES && (counter+1) % INTERIM_RESIDUES <= 2);
		interim_file = (INTERIM_FILES && (counter+1) % INTERIM_FILES == 0);

/* Do a Lucas-Lehmer iteration */

		timers[1] = 0.0;
		start_timer (timers, 1);

/* If we are recovering from a big roundoff error, then run one */
/* iteration using extra multiplies to reduce the roundoff error. */
/* This shouldn't run into any roundoff problems and will protect us from */
/* roundoff errors up to (1.0 - 0.40625). */

		if (maxerr_recovery_mode && counter == last_counter) {
			gwsetnormroutine (&lldata.gwdata, 0, echk, 0);
			gwstartnextfft (&lldata.gwdata, 0);
			lucas_fixup (&lldata, p);
			gwsquare_carefully (&lldata.gwdata, lldata.lldata);
			maxerr_recovery_mode = 0;
/* IS THIS STILL NECESSARY???? CAN IT BE FIXED???? IN PRP TOO. */
/* Since our error recovery code cannot cope with an error during a careful */
/* iteration, make sure the error variable is cleared.  This shouldn't */
/* ever happen, but two users inexplicably ran into this problem. */
			gw_clear_error (&lldata.gwdata);
			echk = 0;
		}

/* Otherwise, do a normal iteration */

#ifndef SERVER_TESTING
		else {
			gwsetnormroutine (&lldata.gwdata, 0, echk, 0);
			gwstartnextfft (&lldata.gwdata,
					!saving && !maxerr_recovery_mode && !Jacobi_testing &&
					!sending_residue && !interim_residue && !interim_file &&
					counter+1 != p);
			lucas_fixup (&lldata, p);
			gwsquare (&lldata.gwdata, lldata.lldata);
		}
#endif

/* End iteration timing and increase count of iterations completed */

		end_timer (timers, 1);
		timers[0] += timers[1];
		iters++;

/* Update min/max round-off error */

		if (echk) {
			if (gw_get_maxerr (&lldata.gwdata) < reallyminerr && counter > 30)
				reallyminerr = gw_get_maxerr (&lldata.gwdata);
			if (gw_get_maxerr (&lldata.gwdata) > reallymaxerr)
				reallymaxerr = gw_get_maxerr (&lldata.gwdata);
		}

/* If the sum of the output values is an error (such as infinity) */
/* then raise an error. */

		if (gw_test_illegal_sumout (&lldata.gwdata)) {
			sprintf (buf, ERRMSG0, counter, p, ERRMSG1A);
			OutputBoth (thread_num, buf);
			inc_error_count (2, &error_count);
			sleep5 = TRUE;
			goto restart;
		}

/* Check that the sum of the input numbers squared is approximately */
/* equal to the sum of unfft results.  Since this check may not */
/* be perfect, check for identical results after a restart. */

		if (gw_test_mismatched_sums (&lldata.gwdata)) {
			if (counter == last_counter &&
			    gwsuminp (&lldata.gwdata, lldata.lldata) == last_suminp &&
			    gwsumout (&lldata.gwdata, lldata.lldata) == last_sumout) {
				OutputBoth (thread_num, ERROK);
				inc_error_count (3, &error_count);
				gw_clear_error (&lldata.gwdata);
			} else {
				char	msg[100];
				sprintf (msg, ERRMSG1B,
					 gwsuminp (&lldata.gwdata, lldata.lldata),
					 gwsumout (&lldata.gwdata, lldata.lldata));
				sprintf (buf, ERRMSG0, counter, p, msg);
				OutputBoth (thread_num, buf);
				last_counter = counter;
				last_suminp = gwsuminp (&lldata.gwdata, lldata.lldata);
				last_sumout = gwsumout (&lldata.gwdata, lldata.lldata);
				inc_error_count (0, &error_count);
				sleep5 = TRUE;
				goto restart;
			}
		}

/* Check for excessive roundoff error.  If round off is too large, repeat */
/* the iteration to see if this was a hardware error.  If it was repeatable */
/* then repeat the iteration using a safer, slower method.  This can */
/* happen when operating near the limit of an FFT. */

		if (echk && gw_get_maxerr (&lldata.gwdata) > allowable_maxerr) {
			if (counter == last_counter && gw_get_maxerr (&lldata.gwdata) == last_maxerr) {
				OutputBoth (thread_num, ERROK);
				inc_error_count (3, &error_count);
				gw_clear_error (&lldata.gwdata);
				OutputBoth (thread_num, ERRMSG5);
				maxerr_recovery_mode = 1;
				sleep5 = FALSE;
				goto restart;
			} else {
				char	msg[100];
				sprintf (msg, ERRMSG1C, gw_get_maxerr (&lldata.gwdata), allowable_maxerr);
				sprintf (buf, ERRMSG0, counter, p, msg);
				OutputBoth (thread_num, buf);
				last_counter = counter;
				last_maxerr = gw_get_maxerr (&lldata.gwdata);
				inc_error_count (1, &error_count);
				sleep5 = FALSE;
				goto restart;
			}
		}

/* Check if the units_bit is corrupt.  This will make sure we are always */
/* subtracting 2 from the FFT data.  If the FFT data was mysteriously zeroed */
/* and the units_bit value was corrupt then we could get a false positive */
/* result.  With this fix we should get into a safe -2, 2, 2, 2 loop. */

		if (lldata.units_bit >= p) {
			sprintf (buf, ERRMSG0, counter, p, ERRMSG1D);
			OutputBoth (thread_num, buf);
			inc_error_count (6, &error_count);
			sleep5 = TRUE;
			goto restart;
		}

/* Check the Jacobi symbol */

		if (Jacobi_testing && !jacobi_test (thread_num, p, &lldata)) {
			sprintf (buf, ERRMSG0, counter, p, ERRMSG1G);
			OutputBoth (thread_num, buf);
			inc_error_count (4, &error_count);
			sleep5 = FALSE;
			goto restart;
		}

/* Check if the FFT data has been zeroed. This will help reduce the chances */
/* of another false positive being reported. */

#ifndef SERVER_TESTING
		if (*addr1 == 0.0 && p > 1000 &&
		    counter > 50 && counter < p-2 && counter != last_counter) {
			unsigned long i;
			for (i = 2; ; i++) {
				if (*addr (&lldata.gwdata, lldata.lldata, i) != 0.0) break;
				if (i == 50) {
					sprintf (buf, ERRMSG0, counter, p, ERRMSG1F);
					OutputBoth (thread_num, buf);
					inc_error_count (5, &error_count);
					last_counter = counter;
					sleep5 = TRUE;
					goto restart;
				}
			}
		}
#endif

/* Update counter, percentage complete */

		counter++;
		w->pct_complete = (double) counter * inverse_p;

/* Output the title every so often */

		actual_frequency = (int) (ITER_OUTPUT * output_title_frequency);
		if (actual_frequency < 1) actual_frequency = 1;
		if (counter % actual_frequency == 0 || first_iter_msg) {
			sprintf (buf, "%.*f%% of M%ld", (int) PRECISION, trunc_percent (w->pct_complete), p);
			title (thread_num, buf);
		}

/* Print a message every so often */

		actual_frequency = (int) (ITER_OUTPUT * output_frequency);
		if (actual_frequency < 1) actual_frequency = 1;
		if (counter % actual_frequency == 0 || first_iter_msg) {
			sprintf (buf, "Iteration: %ld / %ld [%.*f%%]", counter, p, (int) PRECISION, trunc_percent (w->pct_complete));
			/* Append a short form total errors message */
			if (error_count_messages == 1)
				make_error_count_message (error_count, error_count_messages,
							  buf + strlen (buf),
							  (int) (sizeof (buf) - strlen (buf)));
			/* Truncate first message */
			if (first_iter_msg) {
				strcat (buf, ".\n");
				clear_timer (timers, 0);
				first_iter_msg = FALSE;
			}
			/* In v28.5 and later, format a consise message including the ETA */
			else if (!CLASSIC_OUTPUT) {
				double speed;
				/* Append roundoff error */
				if ((OUTPUT_ROUNDOFF || ERRCHK) && reallymaxerr >= 0.001) {
					sprintf (buf+strlen(buf), ", roundoff: %5.3f", reallymaxerr);
					if (!CUMULATIVE_ROUNDOFF) reallyminerr = 1.0, reallymaxerr = 0.0;
				}
				/* Append ms/iter */
				speed = timer_value (timers, 0) / (double) iters;
				sprintf (buf+strlen(buf), ", ms/iter: %6.3f", speed * 1000.0);
				clear_timer (timers, 0);
				iters = 0;
				/* Append ETA */
				formatETA ((p - counter) * speed, buf+strlen(buf));
				strcat (buf, "\n");
			}
			/* Format the classic (pre-v28.5) message */
			else {
				/* Append optional roundoff message */
				if (ERRCHK && counter > 30) {
					sprintf (buf+strlen(buf), ".  Round off: %10.10f to %10.10f", reallyminerr, reallymaxerr);
					if (!CUMULATIVE_ROUNDOFF) reallyminerr = 1.0, reallymaxerr = 0.0;
				}
				if (CUMULATIVE_TIMING) {
					strcat (buf, ".  Total time: ");
					print_timer (timers, 0, buf, TIMER_NL);
				} else {
					strcat (buf, ".  Per iteration time: ");
					divide_timer (timers, 0, iters);
					print_timer (timers, 0, buf, TIMER_NL | TIMER_CLR);
					iters = 0;
				}
			}
			OutputStr (thread_num, buf);

/* Output a verbose message showing the error counts.  This way a user is likely to */
/* notice a problem without reading the results.txt file. */

			if (error_count_messages >= 2 &&
			    make_error_count_message (error_count, error_count_messages, buf, sizeof (buf)))
				OutputStr (thread_num, buf);
		}

/* Print a results file message every so often */

		if (counter % ITER_OUTPUT_RES == 0 || (NO_GUI && stop_reason)) {
			sprintf (buf, "Iteration %ld / %ld\n", counter, p);
			writeResults (buf);
		}

/* Write results to a file every DISK_WRITE_TIME minutes */
/* On error, retry in 10 minutes (it could be a temporary disk-full situation) */

		if (saving) {
			if (! writeLLSaveFile (&lldata, &write_save_file_state, w, counter, error_count)) {
				sprintf (buf, WRITEFILEERR, filename);
				OutputBoth (thread_num, buf);
			}
			if (Jacobi_testing) write_save_file_state.special = 1;
		}

/* If an escape key was hit, write out the results and return */

		if (stop_reason) {
			sprintf (buf, "Stopping primality test of M%ld at iteration %ld [%.*f%%]\n",
				 p, counter, (int) PRECISION, trunc_percent (w->pct_complete));
			OutputStr (thread_num, buf);
			lucasDone (&lldata);
			return (stop_reason);
		}

/* Send the 64-bit residue to the server at specified interims.  The server will record */
/* the residues for possible verification at a later date.  We could catch suspect computers */
/* or malicious cheaters without doing a full double-check. */

		if (sending_residue) {
			struct primenetAssignmentProgress pkt;
			memset (&pkt, 0, sizeof (pkt));
			strcpy (pkt.computer_guid, COMPUTER_GUID);
			pkt.cpu_num = thread_num;
			strcpy (pkt.assignment_uid, w->assignment_uid);
			strcpy (pkt.stage, w->stage);
			pkt.pct_complete = w->pct_complete * 100.0;
			pkt.end_date = (unsigned long) work_estimate (thread_num, w);
			pkt.next_update = (uint32_t) (DAYS_BETWEEN_CHECKINS * 86400.0);
			pkt.fftlen = w->fftlen;
			pkt.iteration = counter - 2;
			generateResidue64 (&lldata, p, &high32, &low32);
			sprintf (pkt.residue, "%08lX%08lX", high32, low32);
			sprintf (pkt.error_count, "%08lX", error_count);
			spoolMessage (-PRIMENET_ASSIGNMENT_PROGRESS, &pkt);
		}

/* Output the 64-bit residue at specified interims.  Also output the */
/* residues for the next two iterations so that we can compare our */
/* residues to programs that sensibly start counter at zero or one. */

		if (interim_residue) {
			generateResidue64 (&lldata, p, &high32, &low32);
			sprintf (buf, "M%ld interim Wg%d residue %08lX%08lX at iteration %ld\n", p, PORT, high32, low32, counter);
			OutputBoth (thread_num, buf);
		}

/* Write a save file every INTERIM_FILES iterations. */

		if (interim_file) {
			char	interimfile[32];
			writeSaveFileState state;
			sprintf (interimfile, "%s.%03ld", filename, counter / INTERIM_FILES);
			writeSaveFileStateInit (&state, interimfile, 0);
			state.num_ordinary_save_files = 99;
			writeLLSaveFile (&lldata, &state, w, counter, error_count);
		}

/* If ten iterations take 40% longer than a typical iteration, then */
/* assume a foreground process is running and sleep for a short time */
/* to give the foreground process more CPU time.  Even though a foreground */
/* process runs at higher priority, hyperthreading will cause this */
/* program to run at an equal priority, hurting responsiveness. */

		if (HYPERTHREADING_BACKOFF && p > 10000000) {
			if (timers[1] < best_iteration_time)
				best_iteration_time = timers[1];
			if (timers[1] > 1.40 * best_iteration_time) {
				if (slow_iteration_count == 10) {
					sprintf (buf, "Pausing %lu seconds.\n", HYPERTHREADING_BACKOFF);
					OutputStr (thread_num, buf);
					Sleep (HYPERTHREADING_BACKOFF * 1000);
				}
				slow_iteration_count++;
			} else
				slow_iteration_count = 0;
		}
	}

/* Check for a successful completion */
/* We found a prime if result is zero */
/* Note that all values of -1 is the same as zero */

	rc = generateResidue64 (&lldata, p, &high32, &low32);
	if (rc < 0) {
		sprintf (buf, ERRMSG0, counter, p, ERRMSG1E);
		OutputBoth (thread_num, buf);
		inc_error_count (2, &error_count);
		sleep5 = TRUE;
		goto restart;
	}
	isPrime = (rc == 0);

/* Format the output message */

	if (isPrime)
		sprintf (buf, "M%ld is prime! Wg%d: %08lX,%08lX\n", p, PORT, SEC1 (p), error_count);
	else
		sprintf (buf,
			 "M%ld is not prime. Res64: %08lX%08lX. Wg%d: %08lX,%ld,%08lX\n",
			 p, high32, low32, PORT,
			 SEC2 (p, high32, low32, lldata.units_bit, error_count),
			 lldata.units_bit, error_count);
	OutputStr (thread_num, buf);
	formatMsgForResultsFile (buf, w);
	rc = writeResults (buf);

/* Send results to the server if they might possibly be of interest */

	if (p > 1000000 && (!isPrime || !isKnownMersennePrime (p))) {
		struct primenetAssignmentResult pkt;
		memset (&pkt, 0, sizeof (pkt));
		strcpy (pkt.computer_guid, COMPUTER_GUID);
		strcpy (pkt.assignment_uid, w->assignment_uid);
		strcpy (pkt.message, buf);
		pkt.result_type = isPrime ? PRIMENET_AR_LL_PRIME : PRIMENET_AR_LL_RESULT;
		pkt.n = p;
		sprintf (pkt.residue, "%08lX%08lX", high32, low32);
		pkt.shift_count = lldata.units_bit;
		sprintf (pkt.error_count, "%08lX", error_count);
		pkt.fftlen = gwfftlen (&lldata.gwdata);
		pkt.done = TRUE;
		spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
	}

/* Delete the continuation files - assuming the results file write */
/* was successful. */

	if (!isPrime || isKnownMersennePrime (p)) {
		if (rc) unlinkSaveFiles (&write_save_file_state);
	}

/* Clean up */

	lucasDone (&lldata);

/* Output good news to the screen in an infinite loop */

	if (isPrime && !SILENT_VICTORY && !isKnownMersennePrime (p)) {
		gwthread thread_handle;
		gwthread_create (&thread_handle, &good_news, (void *) p);
	}

/* All done */

	return (STOP_WORK_UNIT_COMPLETE);

/* An error occured, output a message saying we are restarting, sleep, */
/* then try restarting at last save point. */

restart:if (sleep5) OutputBoth (thread_num, ERRMSG2);
	OutputBoth (thread_num, ERRMSG3);

/* Save the incremented error count to be used in the restart rather than the error count read from a save file */

	restart_error_count = error_count;

/* Sleep five minutes before restarting */

	if (sleep5) {
		stop_reason = SleepFive (thread_num);
		if (stop_reason) return (stop_reason);
	}

/* Return so that last continuation file is read in */

	lucasDone (&lldata);
	goto begin;
}

/*********************/
/* Torture test code */
/*********************/

static const char TORTURE1[] = "Beginning a continuous self-test on your computer.\n";
#if defined (__linux__) || defined (__FreeBSD__) || defined (__EMX__)
static const char TORTURE2[] = "Please read stress.txt.  Hit ^C to end this test.\n";
#else
static const char TORTURE2[] = "Please read stress.txt.  Choose Test/Stop to end this test.\n";
#endif
static const char SELF1[] = "Test %i, %i Lucas-Lehmer iterations of M%ld using %s.\n";
static const char SELFFAIL[] = "FATAL ERROR: Final result was %08lX, expected: %08lX.\n";
static const char SELFFAIL1[] = "ERROR: ILLEGAL SUMOUT\n";
static const char SELFFAIL2[] = "FATAL ERROR: Resulting sum was %.16g, expected: %.16g\n";
static const char SELFFAIL3[] = "FATAL ERROR: Rounding was %.10g, expected less than 0.4\n";
static const char SELFFAIL4[] = "Possible hardware failure, consult readme.txt file, restarting test.\n";
static const char SELFFAIL5[] = "Hardware failure detected, consult stress.txt file.\n";
static const char SELFFAIL6[] = "Maximum number of warnings exceeded.\n";

static const char SELFPASS[] = "Self-test %i%s passed!\n";
//static const char SelfTestIniMask[] = "SelfTest%iPassed";

struct self_test_info {
	unsigned long p;
	unsigned long iters;
	unsigned long reshi;
};

#define MAX_SELF_TEST_ITERS	405
const struct self_test_info SELF_TEST_DATA[MAX_SELF_TEST_ITERS] = {
{560000001, 100, 0x7F853A0A}, {420000001, 150, 0x89665E7E},
{280000001, 200, 0xC32CAD46}, {210000001, 300, 0x89823329},
{140000001, 400, 0x15EF4F24}, {110000001, 500, 0x893C9000},
{78643201, 400, 0x2D9C8904}, {78643199, 400, 0x7D469182},
{75497473, 400, 0x052C7FD8}, {75497471, 400, 0xCCE7495D},
{71303169, 400, 0x467A9338}, {71303167, 400, 0xBBF8B37D},
{68157441, 400, 0xBE71E616}, {68157439, 400, 0x93A71CC2},
{66060289, 400, 0xF296BB99}, {66060287, 400, 0x649EEF2A},
{62390273, 400, 0xBC8DFC27}, {62390271, 400, 0xDE7D5B5E},
{56623105, 400, 0x0AEBF972}, {56623103, 400, 0x1BA96297},
{53477377, 400, 0x5455F347}, {53477375, 400, 0xCE1C7F78},
{50331649, 400, 0x3D746AC8}, {50331647, 400, 0xE23F2DE6},
{49807361, 400, 0xB43EF4C5}, {49807359, 400, 0xA8BEB02D},
{47185921, 400, 0xD862563C}, {47185919, 400, 0x17281086},
{41943041, 400, 0x0EDA1F92}, {41943039, 400, 0xDE6911AE},
{39845889, 400, 0x43D8A96A}, {39845887, 400, 0x3D118E8F},
{37748737, 400, 0x38261154}, {37748735, 400, 0x22B34CD2},
{35651585, 400, 0xB0E48D2E}, {35651583, 400, 0xCC3340C6},
{34865153, 400, 0xD2C00E6C}, {34865151, 400, 0xFA644F69},
{33030145, 400, 0x83E5738D}, {33030143, 400, 0x6EDBC5B5},
{31195137, 400, 0xFF9591CF}, {31195135, 400, 0x04577C70},
{29884417, 400, 0xACC36457}, {29884415, 400, 0xC0FE7B1E},
{28311553, 400, 0x780EB8F5}, {28311551, 400, 0xE6D128C3},
{26738689, 400, 0x09DC45B0}, {26738687, 400, 0xDC7C074A},
{24903681, 400, 0xA482CF1E}, {24903679, 400, 0x4B3F5121},
{23592961, 400, 0xAFE3C198}, {23592959, 400, 0xCF9AD48C},
{20971521, 400, 0x304EC13B}, {20971519, 400, 0x9C4E157E},
{19922945, 400, 0x83FE36D9}, {19922943, 400, 0x9C60E7A2},
{18874369, 400, 0x83A9F8CB}, {18874367, 400, 0x5A6E22E0},
{17825793, 400, 0xF3A90A5E}, {17825791, 400, 0x6477CA76},
{17432577, 400, 0xCAB36E6A}, {17432575, 400, 0xB8F814C6},
{16515073, 400, 0x91EFCB1C}, {16515071, 400, 0xA0C35CD9},
{15597569, 400, 0x12E057AD}, {15597567, 400, 0xC4EFAEFD},
{14942209, 400, 0x1C912A7B}, {14942207, 400, 0xABA9EA6E},
{14155777, 400, 0x4A943A4E}, {14155775, 400, 0x00789FB9},
{13369345, 400, 0x27A041EE}, {13369343, 400, 0xA8B01A41},
{12451841, 400, 0x4DC891F6}, {12451839, 400, 0xA75BF824},
{11796481, 400, 0xFDD67368}, {11796479, 400, 0xE0237D19},
{10485761, 400, 0x15419597}, {10485759, 400, 0x154D473B},
{10223617, 400, 0x26039EB7}, {10223615, 400, 0xC9DFB1A4},
{9961473, 400, 0x3EB29644}, {9961471, 400, 0xE2AB9CB2},
{9437185, 400, 0x42609D65}, {9437183, 400, 0x77ED0792},
{8716289, 400, 0xCCA0C17B}, {8716287, 400, 0xD47E0E85},
{8257537, 400, 0x80B5C05F}, {8257535, 400, 0x278AE556},
{7798785, 400, 0x55A2468D}, {7798783, 400, 0xCF62032E},
{7471105, 400, 0x0AE03D3A}, {7471103, 400, 0xD8AB333B},
{7077889, 400, 0xC516359D}, {7077887, 400, 0xA23EA7B3},
{6684673, 400, 0xA7576F00}, {6684671, 400, 0x057E57F4},
{6422529, 400, 0xC779D2C3}, {6422527, 400, 0xA8263D37},
{6225921, 400, 0xB46AEB2F}, {6225919, 400, 0xD0A5FD5F},
{5898241, 400, 0xE46E76F9}, {5898239, 400, 0x29ED63B2},
{5505025, 400, 0x83566CC3}, {5505023, 400, 0x0B9CBE64},
{5242881, 400, 0x3CC408F6}, {5242879, 400, 0x0EA4D112},
{4980737, 400, 0x6A2056EF}, {4980735, 400, 0xE03CC669},
{4718593, 400, 0x87622D6B}, {4718591, 400, 0xF79922E2},
{4587521, 400, 0xE189A38A}, {4587519, 400, 0x930FF36C},
{4358145, 400, 0xDFEBF850}, {4358143, 400, 0xBB63D330},
{4128769, 400, 0xC0844AD1}, {4128767, 400, 0x25BDBFC3},
{3932161, 400, 0x7A525A7E}, {3932159, 400, 0xF30C9045},
{3735553, 400, 0xFAD79E97}, {3735551, 400, 0x005ED15A},
{3538945, 400, 0xDDE5BA46}, {3538943, 400, 0x15ED5982},
{3342337, 400, 0x1A6E87E9}, {3342335, 400, 0xECEEA390},
{3276801, 400, 0x3341C77F}, {3276799, 400, 0xACA2EE28},
{3112961, 400, 0x2BDF9D2B}, {3112959, 400, 0xA0AC8635},
{2949121, 400, 0x36EDB768}, {2949119, 400, 0x53FD5473},
{2785281, 400, 0x66816C94}, {2785279, 400, 0x059E8D6B},
{2654999, 400, 0x07EE900D}, {2621441, 400, 0x2BC1DACD},
{2621439, 400, 0xBCBA58F1}, {2653987, 400, 0xB005CACC},
{2651879, 400, 0x38DCD06B}, {2654003, 400, 0x1ED556E7},
{2620317, 400, 0x09DB64F8}, {2539613, 400, 0x4146EECA},
{2573917, 400, 0x939DA3B3}, {2359297, 400, 0x73A131F0},
{2359295, 400, 0x53A92203}, {2646917, 400, 0x71D4E5A2},
{2605473, 400, 0xE11637FC}, {2495213, 400, 0x89D80370},
{2540831, 400, 0x2CF01FBB}, {2654557, 400, 0x4106F46F},
{2388831, 400, 0xA508B5A7}, {2654777, 400, 0x9E744AA3},
{2584313, 400, 0x800E9A61}, {2408447, 400, 0x8C91E8AA},
{2408449, 400, 0x437ECC01}, {2345677, 400, 0x60AEE9C2},
{2332451, 400, 0xAB209667}, {2330097, 400, 0x3FB88055},
{2333851, 400, 0xFE4ECF19}, {2444819, 400, 0x56BF33C5},
{2555671, 400, 0x9DC03527}, {2654333, 400, 0xE81BCF40},
{2543123, 400, 0x379CA95D}, {2432123, 400, 0x5952676A},
{2321123, 400, 0x24DCD25F}, {2654227, 400, 0xAC3B7F2B},
{2329999, 400, 0xF5E902A5}, {2293761, 400, 0x9E4BBB8A},
{2293759, 400, 0x1901F07B}, {2236671, 400, 0x45EB162A},
{2193011, 400, 0x382B6E4B}, {2329001, 400, 0x4FF052BB},
{2327763, 400, 0x3B315213}, {2325483, 400, 0x0DC5165A},
{2323869, 400, 0xD220E27F}, {2315679, 400, 0xF650BE33},
{2004817, 400, 0xC2FF3440}, {2130357, 400, 0xC25804D8},
{2288753, 400, 0xA4DD9AAD}, {2266413, 400, 0x675257DB},
{2244765, 400, 0xC08FF487}, {2222517, 400, 0x1A128B22},
{2200339, 400, 0x0EB0E827}, {2328117, 400, 0x0A24673A},
{2329557, 400, 0x2E267692}, {2188001, 400, 0xD012AF6A},
{2166567, 400, 0x509BA41A}, {2144651, 400, 0x54CFC0E6},
{2122923, 400, 0xA47068E6}, {2100559, 400, 0xACFAB4E1},
{2088461, 400, 0xEA01E860}, {2066543, 400, 0x847DF0D0},
{2044767, 400, 0x04225888}, {2022823, 400, 0x6EA34B32},
{2328527, 400, 0xC55E3E05}, {2327441, 400, 0x207C8CEC},
{2326991, 400, 0x0A4F2ACD}, {2009987, 400, 0xE6A59DEF},
{1999999, 400, 0xD645A18F}, {1966081, 400, 0xB88828A1},
{1966079, 400, 0x5BD87C45}, {1998973, 400, 0xCBDD74F7},
{1997651, 400, 0x666B0CB1}, {1675001, 400, 0x50A94DB7},
{1977987, 400, 0x30D1CD1F}, {1955087, 400, 0x5B9426A4},
{1933071, 400, 0x23C1AF0B}, {1911957, 400, 0xF7699248},
{1899247, 400, 0x11C76E04}, {1877431, 400, 0xA3299B39},
{1855067, 400, 0x35243683}, {1833457, 400, 0xCF630DC0},
{1811987, 400, 0x7C7022EC}, {1799789, 400, 0xEFEC47B7},
{1777773, 400, 0x0F16E2D6}, {1755321, 400, 0x1AC5D492},
{1733333, 400, 0x5DA0555E}, {1711983, 400, 0xDC19DA8B},
{1699779, 400, 0x2B44914E}, {1677323, 400, 0x03D3980B},
{1995091, 400, 0x922E555B}, {1993041, 400, 0x0CA8451B},
{1991991, 400, 0xDFFB212D}, {1679779, 400, 0x51D75E0F},
{1684993, 400, 0x048BBCE8}, {1970009, 400, 0x646E0DFA},
{1957445, 400, 0xC8D244ED}, {1999997, 400, 0x5FC899D0},
{1998983, 400, 0x1CD518AA}, {1999007, 400, 0xA9DD8591},
{1674999, 400, 0xDB0169D8}, {1638401, 400, 0xD3F8A8C5},
{1638399, 400, 0xF270D8DD}, {1674997, 400, 0xC824EF15},
{1674551, 400, 0xD844AEAD}, {1674001, 400, 0x8F5EFA50},
{1345001, 400, 0x18EE2E2D}, {1655083, 400, 0x09B30DEE},
{1633941, 400, 0x0B87C8B1}, {1611557, 400, 0x6B57E48D},
{1599549, 400, 0x48EA38B2}, {1577771, 400, 0xCE84D9DC},
{1555947, 400, 0x6797EEF4}, {1533349, 400, 0xD6897409},
{1511861, 400, 0x8A8177AC}, {1499625, 400, 0x56BB6FB3},
{1477941, 400, 0xF3DD8ED3}, {1455931, 400, 0x31A222C7},
{1433069, 400, 0x28F01E1B}, {1411747, 400, 0x680C6E39},
{1399449, 400, 0xB7F01A54}, {1377247, 400, 0xE656F652},
{1355991, 400, 0xB2AA2819}, {1350061, 400, 0x31F9A728},
{1673881, 400, 0xA51D38E4}, {1672771, 400, 0x5474B6F9},
{1671221, 400, 0x2710DDEA}, {1670551, 400, 0x31FC3838},
{1660881, 400, 0x4C5B22C5}, {1650771, 400, 0x998F747B},
{1655001, 400, 0x164659A6}, {1674339, 400, 0xED2D23E2},
{1344999, 400, 0x158AA064}, {1310721, 400, 0x5694A427},
{1310719, 400, 0x258BDDE3}, {1344997, 400, 0x1D059D4F},
{1344551, 400, 0x60606AA3}, {1344001, 400, 0x9AC6AB36},
{1322851, 400, 0x3A000D0A}, {1300993, 400, 0x77CB0184},
{1288771, 400, 0x7431D9E2}, {1266711, 400, 0xB4BC4E8D},
{1244881, 400, 0x48BC9FF9}, {1222991, 400, 0x3F5FC39E},
{1200881, 400, 0xD5DF4944}, {1188441, 400, 0xD9D8968B},
{1166661, 400, 0xD4AB97F4}, {1144221, 400, 0x9940943B},
{1122001, 400, 0x647406B8}, {1100881, 400, 0x3AD40CE0},
{1088511, 400, 0xD578BB51}, {1066837, 400, 0x2F82BFBB},
{1044811, 400, 0x7C6EDDD1}, {1022991, 400, 0x6A1C2DD4},
{1000001, 400, 0x2879748F}, {1343881, 400, 0xB59E8006},
{1342771, 400, 0x87563FFE}, {1341221, 400, 0x29AD6127},
{1340551, 400, 0x17DB4ACB}, {1330881, 400, 0x9642F068},
{942079, 1000, 0xE528A9B0}, {974849, 1000, 0x79791EDB},
{983041, 1000, 0x29216C43}, {901121, 1000, 0x26C4E660},
{917503, 1000, 0x5F244685}, {933889, 1000, 0x62490F57},
{851967, 1000, 0x331AA906}, {860161, 1000, 0x41185F27},
{884735, 1000, 0x7BC7A661}, {802817, 1000, 0xA9645693},
{819199, 1000, 0x48AFB0A5}, {835585, 1000, 0x706437D3},
{753663, 1000, 0x99C43F31}, {778241, 1000, 0x1729A6C4},
{786431, 1000, 0x61080929}, {720897, 1000, 0x1E96863D},
{737279, 1000, 0x1B07A764}, {745473, 1000, 0x7BCE80AA},
{655359, 1000, 0x1107F161}, {659457, 1000, 0x589C16A4},
{688127, 1000, 0xD01E5A85}, {622593, 1000, 0x26F6FC8C},
{630783, 1000, 0x4DD2E603}, {638977, 1000, 0xC88F34B4},
{589823, 1000, 0x0290B60B}, {602113, 1000, 0xEFCD5BA8},
{614399, 1000, 0x6408F880}, {557057, 1000, 0xC30FE589},
{565247, 1000, 0xF4CA3679}, {573441, 1000, 0xF8F039AA},
{532479, 1000, 0x0072FE03}, {540673, 1000, 0xDA0E0D99},
{544767, 1000, 0x62443C6B}, {491521, 1000, 0x3F520DFA},
{516095, 1000, 0xA6BD9423}, {524289, 1000, 0xCD591388},
{466943, 1000, 0xE10EE929}, {471041, 1000, 0x18752F40},
{487423, 1000, 0x933FFF17}, {442369, 1000, 0xC22471C3},
{450559, 1000, 0x025B1320}, {458753, 1000, 0xE296CC00},
{417791, 1000, 0x080C803C}, {425985, 1000, 0xB2095F04},
{430079, 1000, 0x98B1EC61}, {393217, 1000, 0x26DD79ED},
{401407, 1000, 0x2F0F75F9}, {409601, 1000, 0xAEFAC2F8},
{372735, 1000, 0xCB6D00A2}, {376833, 1000, 0x915D5458},
{389119, 1000, 0x6188E38D}, {344065, 1000, 0x4D0C5089},
{360447, 1000, 0x84AC5CFD}, {368641, 1000, 0x72414364},
{319487, 1000, 0x24ED1BE9}, {327681, 1000, 0x3101106A},
{329727, 1000, 0x5BDB69AF}, {307201, 1000, 0x68536CD1},
{311295, 1000, 0x69778074}, {315393, 1000, 0x429D4950},
{286719, 1000, 0x1A31A686}, {294913, 1000, 0xF55727C6},
{301055, 1000, 0x33BDB242}, {272385, 1000, 0xEF6EC4B4},
{278527, 1000, 0x05530FD5}, {282625, 1000, 0x34A4E699},
{262143, 1000, 0xA9638844}, {266241, 1000, 0xE0969CED},
{270335, 1000, 0x14AD54BE}, {243713, 1000, 0xC19AEA91},
{245759, 1000, 0x7538BF0B}, {258049, 1000, 0x73F541AD},
{229375, 1000, 0x6E42B26A}, {233473, 1000, 0x1964F897},
{235519, 1000, 0x661BBC3F}, {215041, 1000, 0x04D5D2F0},
{221183, 1000, 0xA89E7764}, {225281, 1000, 0x20876BED},
{204799, 1000, 0xD20C2126}, {208897, 1000, 0x9D4DCF0E},
{212991, 1000, 0x1FF00E2A}, {194561, 1000, 0x6ED1CB70},
{196607, 1000, 0x3190D5F5}, {200705, 1000, 0xFAD28F5A},
{184319, 1000, 0x360EF08E}, {186369, 1000, 0x0F001482},
{188415, 1000, 0x86FCE4D6}, {164865, 1000, 0x4942B002},
{172031, 1000, 0xC5AF29DB}, {180225, 1000, 0x35D49D74},
{157695, 1000, 0x5422FACF}, {159745, 1000, 0xB5CD03A1},
{163839, 1000, 0x1CA6048E}, {150529, 1000, 0x7412F09C},
{153599, 1000, 0xA9FAAE69}, {155649, 1000, 0xA7B736AF},
{141311, 1000, 0x7A5D0730}, {143361, 1000, 0x580F4DC4},
{147455, 1000, 0x176B299A}, {135169, 1000, 0x65AC10A4},
{136191, 1000, 0xC4591D37}, {139265, 1000, 0xBCE1FC80},
{129023, 1000, 0xAFE1E7A8}, {131073, 1000, 0xC5AAB12F},
{133119, 1000, 0xDE51C35A}, {117761, 1000, 0x054A26F6},
{121855, 1000, 0x55AF2385}, {122881, 1000, 0x652827AC},
{112639, 1000, 0x6FA4DB24}, {114689, 1000, 0x0BBAF161},
{116735, 1000, 0xB85F0E8E}, {106497, 1000, 0xF833D925},
{107519, 1000, 0x80F177D8}, {110593, 1000, 0x1A56AA86},
{100351, 1000, 0x1DE12CE6}, {102401, 1000, 0x19F967B4},
{104447, 1000, 0xF9F3CDFD}
};

#define MAX_SELF_TEST_ITERS2	376
const struct self_test_info SELF_TEST_DATA2[MAX_SELF_TEST_ITERS2] = {
{560000001, 100, 0x7F853A0A}, {420000001, 150, 0x89665E7E},
{280000001, 200, 0xC32CAD46}, {210000001, 300, 0x89823329},
{140000001, 400, 0x15EF4F24}, {110000001, 500, 0x893C9000},
{77497473, 900, 0xF0B43F54}, {76497471, 900, 0xF30AFA95},
{75497473, 900, 0x32D8D3A7}, {75497471, 900, 0x9E689331},
{74497473, 900, 0xD43166A4}, {73497471, 900, 0x639E4F0C},
{72303169, 900, 0x74BDED5C}, {71303169, 900, 0xA2147B5C},
{71303167, 900, 0x717525AB}, {70303167, 900, 0xD716B4F0},
{68060289, 1000, 0xF90C7BFF}, {67060287, 1000, 0xFE9BF47C},
{66060289, 1000, 0x057C60F5}, {66060287, 1000, 0x2ECC97CE},
{65390273, 1000, 0xC55C6369}, {64390271, 1000, 0x48552448},
{63390273, 1000, 0x6FF8CD84}, {62390273, 1000, 0x42ACEB15},
{62390271, 1000, 0x48764DF8}, {61390271, 1000, 0xD5408698},
{57623105, 1200, 0x098B4491}, {56623105, 1200, 0x5E720717},
{56623103, 1200, 0x1980D8BC}, {55623103, 1200, 0xEDD592B6},
{53477377, 1200, 0xBAEF5CCC}, {53477375, 1200, 0x2F296FC8},
{52331647, 1200, 0xA1EAE85D}, {51331649, 1200, 0xE3B39845},
{50331649, 1200, 0x53543DF2}, {50331647, 1200, 0x0049E54B},
{48185921, 1500, 0x78F4AEAA}, {47185921, 1500, 0x4D7FFDDC},
{47185919, 1500, 0x059D196F}, {46185919, 1500, 0x38B1D9AD},
{45943041, 1500, 0x7670FDDF}, {44943039, 1500, 0xA859BBD7},
{43943041, 1500, 0xD673E000}, {42943039, 1500, 0x6B69D8CE},
{41943041, 1500, 0x6E92CE47}, {41943039, 1500, 0x888BEE79},
{39151585, 1900, 0x3B06496C}, {38748737, 1900, 0x6429E0FD},
{38251583, 1900, 0x04AD7F99}, {37748737, 1900, 0x47659BC5},
{37748735, 1900, 0x2DFA41B0}, {36748735, 1900, 0x1A1DA557},
{36251585, 1900, 0x83F23FA8}, {35651585, 1900, 0x3598B4B9},
{35651583, 1900, 0x7E443962}, {35251583, 1900, 0x1CE4D084},
{34230145, 2100, 0x0FDE9717}, {33730143, 2100, 0x54EB5333},
{33030145, 2100, 0xF37897B8}, {33030143, 2100, 0x52B3981B},
{32595137, 2100, 0xA76D0805}, {32095135, 2100, 0xCF443ACD},
{31595137, 2100, 0xA6DEA70A}, {31195137, 2100, 0x0777442D},
{31195135, 2100, 0x9B265F8F}, {30695135, 2100, 0xA3BC760F},
{29311553, 2500, 0xFD1D6D74}, {28811551, 2500, 0xE720BFD3},
{28311553, 2500, 0xA11F75AB}, {28311551, 2500, 0x7E0471E5},
{27738689, 2500, 0xD246DC55}, {27238687, 2500, 0x806A3A62},
{26738689, 2500, 0x8E8450B1}, {26738687, 2500, 0xD4A0DBC9},
{26138689, 2500, 0x47C47755}, {25638687, 2500, 0x7E9C7E8E},
{24903681, 3100, 0x50835AB8}, {24903679, 3100, 0xAE3D2F94},
{24092961, 3100, 0x7B540B4D}, {23892959, 3100, 0xA0D4EC50},
{23592961, 3100, 0x47FBD6FE}, {23592959, 3100, 0x09FD89AB},
{22971521, 3100, 0x99DFEDB9}, {21871519, 3100, 0x35A8B46A},
{20971521, 3100, 0x94C12572}, {20971519, 3100, 0x1F6D3003},
{19922945, 4000, 0x86B106EB}, {19922943, 4000, 0xE1CE3C1A},
{19374367, 4000, 0xD1045A66}, {19174369, 4000, 0x3247CE82},
{18874369, 4000, 0x33BB2689}, {18874367, 4000, 0x6856F21F},
{18474367, 4000, 0x95E2F6FA}, {18274367, 4000, 0x61182009},
{18274369, 4000, 0xB2FD8175}, {18074369, 4000, 0x7F242A6E},
{17432577, 4500, 0x632CAD0B}, {17432575, 4500, 0xC9C79F07},
{17115073, 4500, 0xF2B70D4B}, {16815071, 4500, 0x71B22529},
{16515073, 4500, 0xAB1CC854}, {16515071, 4500, 0xF54D05D7},
{16297569, 4500, 0x6B5F72DA}, {15997567, 4500, 0x9669F188},
{15597569, 4500, 0x352BFCCF}, {15597567, 4500, 0x36B164ED},
{14942209, 5300, 0xEA5DB53B}, {14942207, 5300, 0x6CC650A2},
{14155777, 5300, 0xEB7C125D}, {14155775, 5300, 0xB4C8B09B},
{13969343, 5300, 0x832359A5}, {13669345, 5300, 0x7EE99140},
{13369345, 5300, 0xCDF43471}, {13369343, 5300, 0x343FEA12},
{13069345, 5300, 0x65B17A9B}, {12969343, 5300, 0x063F492B},
{12451841, 6500, 0xCB168E5D}, {12451839, 6500, 0xE91EEB5A},
{12196481, 6500, 0x0A261B7E}, {11796481, 6500, 0x38100A5F},
{11796479, 6500, 0x78FCF8C5}, {11596479, 6500, 0x8C481635},
{11285761, 6500, 0x2580BC8D}, {10885759, 6500, 0x54030992},
{10485761, 6500, 0x054660AA}, {10485759, 6500, 0x50F74AF0},
{9961473, 7800, 0x7991161C}, {9961471, 7800, 0x627F3BEE},
{9837183, 7800, 0xBC67A608}, {9737185, 7800, 0x9A0CBC59},
{9537183, 7800, 0xA6A509A6}, {9437185, 7800, 0x877C09B6},
{9437183, 7800, 0x1D259540}, {9337185, 7800, 0x5EF3F14C},
{9237183, 7800, 0x5780245F}, {9137185, 7800, 0x6C1162A9},
{8716289, 9000, 0x2011133F}, {8716287, 9000, 0xEEEC1181},
{8516289, 9000, 0xF1D93A69}, {8316287, 9000, 0x53D6E3CB},
{8257537, 9000, 0x38DB98D6}, {8257535, 9000, 0x7D1BECA7},
{8098785, 9000, 0x51E9FA27}, {7998783, 9000, 0xF7F14FF2},
{7798785, 9000, 0x8437BC4D}, {7798783, 9000, 0x9E28D8E1},
{7471105, 11000, 0xEFDA89EA}, {7471103, 11000, 0x4061C4BF},
{7377889, 11000, 0x65ABE846}, {7277887, 11000, 0x02B0EBD7},
{7077889, 11000, 0x336E1030}, {7077887, 11000, 0x685B792E},
{6984673, 11000, 0x3AE19FAF}, {6884671, 11000, 0x2A0ED16A},
{6684673, 11000, 0x206A3512}, {6684671, 11000, 0x4FD9980A},
{6225921, 13000, 0x1A922371}, {6225919, 13000, 0xC0F63BD8},
{6198241, 13000, 0xDA664501}, {6098239, 13000, 0xB92015CD},
{5898241, 13000, 0xDA384BD9}, {5898239, 13000, 0x20B59AC8},
{5705025, 13000, 0x941A2DA0}, {5605023, 13000, 0xCFDF5835},
{5505025, 13000, 0x37A6C972}, {5505023, 13000, 0x6252AB5C},
{5120737, 17000, 0x512705D0}, {5030735, 17000, 0x633E3E74},
{4980737, 17000, 0xD8245D49}, {4980735, 17000, 0xFB2C3530},
{4888593, 17000, 0xE3C6EDBC}, {4818591, 17000, 0x89E7FE48},
{4718593, 17000, 0xA23C713D}, {4718591, 17000, 0xC7BA41D6},
{4698593, 17000, 0xA0194103}, {4648591, 17000, 0xD5A50A23},
{4501145, 19000, 0x7BAF4344}, {4458143, 19000, 0x686F6B13},
{4358145, 19000, 0x682E6643}, {4358143, 19000, 0x974DA6CC},
{4298769, 19000, 0x1FC0E577}, {4228767, 19000, 0x46B5F3CD},
{4128769, 19000, 0x59332478}, {4128767, 19000, 0x4AF5C8B8},
{4028769, 19000, 0x542C17CB}, {3978767, 19000, 0x76E41351},
{3835553, 22000, 0x9058FE40}, {3785551, 22000, 0x45EF5C15},
{3735553, 22000, 0x2700B350}, {3735551, 22000, 0x09EDCEAD},
{3688945, 22000, 0x626C29D3}, {3618943, 22000, 0x82B1D4D1},
{3538945, 22000, 0x70331CC6}, {3538943, 22000, 0x00FEB746},
{3342337, 22000, 0x7CEE24AE}, {3342335, 22000, 0x1802D072},
{3242961, 27000, 0xE877F863}, {3172959, 27000, 0x04C9F1F7},
{3112961, 27000, 0x241E93DB}, {3112959, 27000, 0x8D359307},
{2949121, 27000, 0x6B545E09}, {2949119, 27000, 0xAFD6F417},
{2885281, 27000, 0x439E57E6}, {2785281, 27000, 0xB4E40DFE},
{2785279, 27000, 0x3787D3FA}, {2685279, 27000, 0x902967B7},
{2605473, 34000, 0xE21C344E}, {2584313, 34000, 0xFDBCFCB2},
{2573917, 34000, 0x89B5012C}, {2540831, 34000, 0x201BAA90},
{2539613, 34000, 0x2226BA6B}, {2495213, 34000, 0xE3577D9F},
{2408447, 34000, 0x594C9155}, {2388831, 34000, 0x55CE9F16},
{2359297, 34000, 0x09A72A40}, {2359295, 34000, 0x621E8BF9},
{2244765, 39000, 0xEC2F362D}, {2236671, 39000, 0x4B50CA20},
{2222517, 39000, 0x8DA427C0}, {2193011, 39000, 0xD1DE8993},
{2130357, 39000, 0x4B5EBB90}, {2122923, 39000, 0x5F9110FC},
{2100559, 39000, 0xE0CF8904}, {2088461, 39000, 0x26AD1DEA},
{2066543, 39000, 0xB78C9237}, {2004817, 39000, 0x3D7838F8},
{1933071, 46000, 0x86323D21}, {1911957, 46000, 0x500CFEAD},
{1899247, 46000, 0x128667DF}, {1877431, 46000, 0x2A59B6B5},
{1855067, 46000, 0xBE9AABF5}, {1833457, 46000, 0xB84D7929},
{1777773, 46000, 0x771E0A9D}, {1755321, 46000, 0xF93334E3},
{1699779, 46000, 0x07B46DEE}, {1677323, 46000, 0x910E0320},
{1633941, 56000, 0x455509CD}, {1611557, 56000, 0x0F51FA1E},
{1599549, 56000, 0x646A96B0}, {1577771, 56000, 0xA4A21303},
{1555947, 56000, 0x80B84725}, {1533349, 56000, 0x23E9F7B1},
{1477941, 56000, 0x593F208F}, {1455931, 56000, 0x11002C52},
{1433069, 56000, 0x5B641D8B}, {1411747, 56000, 0x5EAE18A8},
{1322851, 75000, 0xD5C50F2E}, {1310721, 75000, 0x855E44A2},
{1310719, 75000, 0xC0836C1F}, {1300993, 75000, 0xF62263D6},
{1288771, 75000, 0x867EBBAB}, {1266711, 75000, 0xBA1FF3BE},
{1244881, 75000, 0xCE8199EB}, {1222991, 75000, 0xCDE49EF5},
{1200881, 75000, 0xC8610F6C}, {1188441, 75000, 0xFC772495},
{1150221, 84000, 0xA3334541}, {1144221, 84000, 0x44307B03},
{1122001, 84000, 0x9B937DCF}, {1108511, 84000, 0x9F3D191E},
{1100881, 84000, 0xBAF4EA2D}, {1096837, 84000, 0xAA9396F1},
{1088511, 84000, 0xB0CB2704}, {1066837, 84000, 0x031F202C},
{1044811, 84000, 0x7EA89CFE}, {1022991, 84000, 0xD42294C8},
{983041, 100000, 0x4052BBC0}, {974849, 100000, 0xB0E9EB07},
{942079, 100000, 0xEE230987}, {933889, 100000, 0x58FA63B0},
{917503, 100000, 0x8B457209}, {901121, 100000, 0xD2325FC4},
{884735, 100000, 0xCBB5A603}, {860161, 100000, 0xBC240C77},
{854735, 100000, 0xE8BE766D}, {851967, 100000, 0x09AD9B74},
{827279, 120000, 0x64B01894}, {819199, 120000, 0xF97F1E2B},
{802817, 120000, 0xC4EDBC3C}, {795473, 120000, 0x046584E0},
{786431, 120000, 0xC6BA553D}, {778241, 120000, 0x856A5147},
{753663, 120000, 0xC7895B4A}, {745473, 120000, 0x42B47EA2},
{737279, 120000, 0x29E477B8}, {720897, 120000, 0x97111FA7},
{662593, 160000, 0x32472A99}, {659457, 160000, 0xEF49D340},
{655359, 160000, 0x75C12C38}, {644399, 160000, 0xDE632783},
{638977, 160000, 0xDCDB98B4}, {630783, 160000, 0x6B8F0706},
{622593, 160000, 0xD732286D}, {614399, 160000, 0x2489EFB3},
{612113, 160000, 0xCAE00EC6}, {602113, 160000, 0x792AD67D},
{580673, 180000, 0xC508CAFA}, {573441, 180000, 0xB0680C2B},
{565247, 180000, 0xF1DBB762}, {557057, 180000, 0x374F647B},
{544767, 180000, 0x3DC41F49}, {540673, 180000, 0x949A4CB7},
{532479, 180000, 0xEA06DC97}, {524289, 180000, 0xA76CE14A},
{522479, 180000, 0xAA8EAC14}, {516095, 180000, 0x04F0CC23},
{501041, 210000, 0xD9F72F62}, {496943, 210000, 0xD62D5380},
{487423, 210000, 0x55ACB2FD}, {471041, 210000, 0xB6AEAB0E},
{466943, 210000, 0x251CDE78}, {458753, 210000, 0xDC40CADB},
{450559, 210000, 0x2AD0CF72}, {442369, 210000, 0x5FF2E46E},
{441041, 210000, 0x1194CC23}, {436943, 210000, 0x0272AF35},
{420217, 270000, 0xD233852A}, {409601, 270000, 0x6F89825C},
{401407, 270000, 0x3D9DE818}, {393217, 270000, 0xDE8E6FF0},
{392119, 270000, 0x30CA58B7}, {389119, 270000, 0x80975797},
{376833, 270000, 0xC75824DB}, {372735, 270000, 0xF8BE0932},
{368641, 270000, 0xA48AC5E3}, {360447, 270000, 0x7DD29C13},
{339487, 340000, 0xA7311A6D}, {335393, 340000, 0xD9704DF2},
{331681, 340000, 0x3316A003}, {329727, 340000, 0xE46D5991},
{327681, 340000, 0xBEDA4A7B}, {319487, 340000, 0xB25C84FF},
{315393, 340000, 0xF5AD1DDA}, {311295, 340000, 0xFE41A12A},
{308295, 340000, 0x03AAC47E}, {307201, 340000, 0xFC08ACCC},
{291913, 380000, 0xC56AB884}, {286719, 380000, 0x248EF622},
{282625, 380000, 0x50A98488}, {280335, 380000, 0x9B64A843},
{278527, 380000, 0x39D5B7DB}, {274335, 380000, 0x48623B41},
{270335, 380000, 0xC04B857A}, {266241, 380000, 0xFE4475F6},
{262143, 380000, 0xADC3ECE9}, {260335, 380000, 0x15B8F9EF},
{250519, 460000, 0xA2FE3B50}, {245759, 460000, 0xC6D800D6},
{245281, 460000, 0x4F23AA34}, {243713, 460000, 0xB30EC823},
{235519, 460000, 0x31FD709E}, {233473, 460000, 0x8FCC69C2},
{231183, 460000, 0xD59255CC}, {229375, 460000, 0x788520D0},
{225281, 460000, 0xD669C8BC}, {221183, 460000, 0x9B915F4B},
{212991, 560000, 0x0555250D}, {210415, 560000, 0x3FC3CCD7},
{208897, 560000, 0x9FF8F462}, {204799, 560000, 0x294EB549},
{200705, 560000, 0x80B1222F}, {196607, 560000, 0x8AB8D945},
{194561, 560000, 0x4140E623}, {188415, 560000, 0xFA0A3453},
{186369, 560000, 0xAC17EAB6}, {184319, 560000, 0x835F341B},
{172031, 800000, 0xF6BD0728}, {163839, 800000, 0x26C78657},
{159745, 800000, 0x6ACBB961}, {157695, 800000, 0x3EA979F3},
{155649, 800000, 0x09C7ADE4}, {153599, 800000, 0xF601EB92},
{147455, 800000, 0x0AA97D21}, {143361, 800000, 0xEA6A01F1},
{141311, 800000, 0x9BB8A6A3}, {135169, 800000, 0xECA55A45}
};

#define MAX_SELF_TEST_ITERS3	488
const struct self_test_info SELF_TEST_DATA3[MAX_SELF_TEST_ITERS3] = {
{900000001, 100, 0xCF8ADC6F}, {800000001, 100, 0x5171F784},
{700000001, 200, 0x52DE4DB3}, {600000001, 200, 0x1E01473F},
{560000001, 400, 0x5D2075F2}, {420000001, 600, 0x76973D8D},
{280000001, 800, 0xA4B0C213}, {210000001, 1200, 0x9B0FEEA5},
{140000001, 1600, 0xEC8F25E6}, {110000001, 2000, 0xD7EE8401},
{77497473, 3600, 0xEE1F9603}, {76497471, 3600, 0xABE435B0},
{75497473, 3600, 0x36285106}, {75497471, 3600, 0xE8CC66CA},
{74497473, 3600, 0x24B8A2BF}, {73497471, 3600, 0xC12E28E9},
{72303169, 3600, 0x51A924BC}, {71303169, 3600, 0x8FB537CB},
{71303167, 3600, 0xB71873A1}, {70303167, 3600, 0x92EFC50B},
{68060289, 4000, 0xA2629086}, {67060287, 4000, 0x23347B16},
{66060289, 4000, 0xDA787057}, {66060287, 4000, 0x0810958A},
{65390273, 4000, 0xAD06FF26}, {64390271, 4000, 0xE3A7F5DB},
{63390273, 4000, 0x874392AC}, {62390273, 4000, 0xB4718A58},
{62390271, 4000, 0x80C10B5F}, {61390271, 4000, 0xCAD8F47A},
{57623105, 4800, 0x1C2BA27E}, {56623105, 4800, 0xBA735E8B},
{56623103, 4800, 0x13519FDB}, {55623103, 4800, 0xE787C20E},
{53477377, 4800, 0xB35788F2}, {53477375, 4800, 0x03E36F38},
{52331647, 4800, 0xDC9F1FA1}, {51331649, 4800, 0x82533823},
{50331649, 4800, 0x97F22401}, {50331647, 4800, 0x5A2FDCC0},
{48185921, 6000, 0x966A35F6}, {47185921, 6000, 0xD8378EF6},
{47185919, 6000, 0xD04DD7C3}, {46185919, 6000, 0x3BA8288B},
{45943041, 6000, 0xFF87BC35}, {44943039, 6000, 0x726253F8},
{43943041, 6000, 0x8E343AC4}, {42943039, 6000, 0xADF105FF},
{41943041, 6000, 0xE0C8040C}, {41943039, 6000, 0x5EF2E3E9},
{39151585, 7600, 0x294D16AC}, {38748737, 7600, 0xBA261FA4},
{38251583, 7600, 0xA64744BA}, {37748737, 7600, 0xCEA0A996},
{37748735, 7600, 0x71246EC6}, {36748735, 7600, 0xDF0D4C96},
{36251585, 7600, 0x6941330C}, {35651585, 7600, 0x9454919C},
{35651583, 7600, 0xE953A8B3}, {35251583, 7600, 0x95E45098},
{34230145, 8400, 0x0FF2D27E}, {33730143, 8400, 0xA815C3CD},
{33030145, 8400, 0x2968002F}, {33030143, 8400, 0x4AFDF43B},
{32595137, 8400, 0x979CF919}, {32095135, 8400, 0x7C0E8693},
{31595137, 8400, 0x6FD95140}, {31195137, 8400, 0xAA6AD58C},
{31195135, 8400, 0x65EE1BF7}, {30695135, 8400, 0x9D10BC3A},
{29311553, 10000, 0xB8C54183}, {28811551, 10000, 0xC70F9D7E},
{28311553, 10000, 0xEA018EED}, {28311551, 10000, 0x43E2096F},
{27738689, 10000, 0x0EA59538}, {27238687, 10000, 0xC53169EE},
{26738689, 10000, 0x9F98CF04}, {26738687, 10000, 0x733122D3},
{26138689, 10000, 0xD88162ED}, {25638687, 10000, 0x6ADB6B49},
{24903681, 12400, 0xEF9EC005}, {24903679, 12400, 0xAB56E004},
{24092961, 12400, 0x3518F8DD}, {23892959, 12400, 0xE0AEFA13},
{23592961, 12400, 0xD1EC53D7}, {23592959, 12400, 0xB006AE40},
{22971521, 12400, 0xA8964CC4}, {21871519, 12400, 0x4DDF7551},
{20971521, 12400, 0x8927FFB4}, {20971519, 12400, 0x7B3217C2},
{19922945, 16000, 0x069C3DCD}, {19922943, 16000, 0xBED8A46E},
{19374367, 16000, 0x11A21885}, {19174369, 16000, 0x2BB5AEAD},
{18874369, 16000, 0xF47D9EC1}, {18874367, 16000, 0xC342E089},
{18474367, 16000, 0x8AC5B7C8}, {18274367, 16000, 0x4DB0F691},
{18274369, 16000, 0x9886B1C9}, {18074369, 16000, 0x241D5A65},
{17432577, 18000, 0xE7FEF929}, {17432575, 18000, 0xE0673389},
{17115073, 18000, 0xCA8909F8}, {16815071, 18000, 0x4C1D976F},
{16515073, 18000, 0xE86FAE0C}, {16515071, 18000, 0x37F5DF1E},
{16297569, 18000, 0x82A0AF96}, {15997567, 18000, 0x321905E4},
{15597569, 18000, 0x2790951D}, {15597567, 18000, 0xFD88F93B},
{14942209, 21000, 0xE9467E64}, {14942207, 21000, 0x781D4424},
{14155777, 21000, 0xBA64B1E8}, {14155775, 21000, 0xF88B7AAE},
{13969343, 21000, 0xD091E8C3}, {13669345, 21000, 0xE57FED05},
{13369345, 21000, 0xCEEEA179}, {13369343, 21000, 0xBB87F46F},
{13069345, 21000, 0x47222D3F}, {12969343, 21000, 0x477EEFE4},
{12451841, 26000, 0x9A1DC942}, {12451839, 26000, 0x8FEFE60F},
{12196481, 26000, 0x1AD3B450}, {11796481, 26000, 0x6A42C88D},
{11796479, 26000, 0x1A3C83A4}, {11596479, 26000, 0x69D18B9B},
{11285761, 26000, 0x6980EFB6}, {10885759, 26000, 0x223C49A6},
{10485761, 26000, 0xBD0AFF34}, {10485759, 26000, 0xD4216A83},
{9961473, 31000, 0x25DE6210}, {9961471, 31000, 0x2FE72634},
{9837183, 31000, 0x44128AF8}, {9737185, 31000, 0x84C70161},
{9537183, 31000, 0x017BE747}, {9437185, 31000, 0x3D38D6E4},
{9437183, 31000, 0xCF2C58C4}, {9337185, 31000, 0x13BFB2D4},
{9237183, 31000, 0xBBC6391C}, {9137185, 31000, 0x23AF0A31},
{8716289, 36000, 0x10FB9FB7}, {8716287, 36000, 0xDF905C4F},
{8516289, 36000, 0xCB8D21BD}, {8316287, 36000, 0xC61BC2BA},
{8257537, 36000, 0x2F93BEA5}, {8257535, 36000, 0xA9B6681A},
{8098785, 36000, 0x7CEFE90D}, {7998783, 36000, 0x32CA4DC8},
{7798785, 36000, 0xB2669EFF}, {7798783, 36000, 0xF2D393AC},
{7471105, 44000, 0x3D4F6CBB}, {7471103, 44000, 0x51F68987},
{7377889, 44000, 0xD3F710E4}, {7277887, 44000, 0xAF76194F},
{7077889, 44000, 0x815E3804}, {7077887, 44000, 0x2C55F47D},
{6984673, 44000, 0xE530552B}, {6884671, 44000, 0x96085903},
{6684673, 44000, 0x5143D5DB}, {6684671, 44000, 0xD153D55E},
{6225921, 52000, 0xF11B3E86}, {6225919, 52000, 0x26AEF35D},
{6198241, 52000, 0x55A1AD52}, {6098239, 52000, 0xEE20AC08},
{5898241, 52000, 0x024AA620}, {5898239, 52000, 0x36EC9FDB},
{5705025, 52000, 0x87610A79}, {5605023, 52000, 0xBA409794},
{5505025, 52000, 0x0D4AD8BF}, {5505023, 52000, 0xAA82E4D6},
{5120737, 68000, 0xF6376191}, {5030735, 68000, 0x6608D7A5},
{4980737, 68000, 0xE0F7D92F}, {4980735, 68000, 0x8EFD0C10},
{4888593, 68000, 0xF25A28E9}, {4818591, 68000, 0x5EF8173F},
{4718593, 68000, 0x8A2349A7}, {4718591, 68000, 0xD6782279},
{4698593, 68000, 0xE695F8C3}, {4648591, 68000, 0xEEAC3CB7},
{4501145, 76000, 0x9EA735B3}, {4458143, 76000, 0x5D196BB0},
{4358145, 76000, 0x69BA2CCC}, {4358143, 76000, 0x9C4DD97B},
{4298769, 76000, 0xAA0A48AD}, {4228767, 76000, 0xD3AF13A3},
{4128769, 76000, 0xCC2E5548}, {4128767, 76000, 0xB2F51617},
{4028769, 76000, 0x6186CC09}, {3978767, 76000, 0x40CC887E},
{3835553, 88000, 0xE3CA2ED9}, {3785551, 88000, 0x1BD285F6},
{3735553, 88000, 0xAF0621FF}, {3735551, 88000, 0xBC97EF83},
{3688945, 88000, 0xBE99894A}, {3618943, 88000, 0x9D3E55C1},
{3538945, 88000, 0x9757CD7F}, {3538943, 88000, 0xB3AA0A96},
{3342337, 88000, 0xE78AC3D0}, {3342335, 88000, 0x6127F902},
{3242961, 110000, 0x0722ADC3}, {3172959, 110000, 0xA4F278FB},
{3112961, 110000, 0x98E79B6B}, {3112959, 110000, 0x3EC57BE5},
{2949121, 110000, 0x7E5BA333}, {2949119, 110000, 0xE6D8CF29},
{2885281, 110000, 0x4F575F34}, {2785281, 110000, 0x73483675},
{2785279, 110000, 0x95FDDD37}, {2685279, 110000, 0x018291EF},
{2605473, 140000, 0xB0C85136}, {2584313, 140000, 0x90790AD6},
{2573917, 140000, 0x303B334A}, {2540831, 140000, 0x031C1AA0},
{2539613, 140000, 0x79A266C8}, {2495213, 140000, 0x18EE9970},
{2408447, 140000, 0x7B7030D4}, {2388831, 140000, 0x3339B0E9},
{2359297, 140000, 0x4B5D9EF4}, {2359295, 140000, 0xA8FD205D},
{2244765, 160000, 0xE719BC36}, {2236671, 160000, 0x642AE29B},
{2222517, 160000, 0x1E20BD07}, {2193011, 160000, 0x3C64988F},
{2130357, 160000, 0xAA1D86BC}, {2122923, 160000, 0x42499686},
{2100559, 160000, 0x31F3C1EB}, {2088461, 160000, 0xE48241A0},
{2066543, 160000, 0x3BBBFBD6}, {2004817, 160000, 0x5F9B943D},
{1933071, 180000, 0x09344960}, {1911957, 180000, 0x66F5EC79},
{1899247, 180000, 0x6D8B1D9B}, {1877431, 180000, 0x325EB183},
{1855067, 180000, 0xCB9EED7F}, {1833457, 180000, 0x0663527F},
{1777773, 180000, 0xB78DA358}, {1755321, 180000, 0xDE573EE9},
{1699779, 180000, 0x8745CD26}, {1677323, 180000, 0xE138A3E2},
{1633941, 220000, 0x8D116786}, {1611557, 220000, 0x8CD83629},
{1599549, 220000, 0xD950AEE1}, {1577771, 220000, 0xB592C606},
{1555947, 220000, 0xD7C183D6}, {1533349, 220000, 0xBAE10734},
{1477941, 220000, 0x903394EC}, {1455931, 220000, 0x22203D42},
{1433069, 220000, 0x1CB8E61C}, {1411747, 220000, 0x6104BE9F},
{1322851, 300000, 0x20B81597}, {1310721, 300000, 0xE89D646E},
{1310719, 300000, 0x41AE4CA1}, {1300993, 300000, 0xD34E4497},
{1288771, 300000, 0x128E16D1}, {1266711, 300000, 0x840497CE},
{1244881, 300000, 0x8AFB3D24}, {1222991, 300000, 0xDAFAE5FB},
{1200881, 300000, 0x5190783B}, {1188441, 300000, 0xF5FD938D},
{1150221, 330000, 0x7311E3A0}, {1144221, 330000, 0x6A2EB001},
{1122001, 330000, 0x25448CBB}, {1108511, 330000, 0x36C4124A},
{1100881, 330000, 0x957930CB}, {1096837, 330000, 0x39C43852},
{1088511, 330000, 0x79B0E4DB}, {1066837, 330000, 0x4FDDE395},
{1044811, 330000, 0x70108FEE}, {1022991, 330000, 0xACCCA430},
{983041, 400000, 0x205E1EF2}, {974849, 400000, 0x2E8CEF15},
{942079, 400000, 0xCDF36D31}, {933889, 400000, 0x1A75EF3C},
{917503, 400000, 0x91D50B39}, {901121, 400000, 0x5E87DF64},
{884735, 400000, 0xE12C485D}, {860161, 400000, 0x524E6891},
{854735, 400000, 0x8B9BF82E}, {851967, 400000, 0xAF790945},
{827279, 480000, 0xE880E7E1}, {819199, 480000, 0x6A230C26},
{802817, 480000, 0x62EA07D7}, {795473, 480000, 0x0FE31D56},
{786431, 480000, 0xCF4CE6EF}, {778241, 480000, 0x8E467FCA},
{753663, 480000, 0x85D18DAE}, {745473, 480000, 0x06C55332},
{737279, 480000, 0xE19FE986}, {720897, 480000, 0xC83C96AA},
{662593, 640000, 0x42DD71CD}, {659457, 640000, 0x1B973A76},
{655359, 640000, 0x4B3D2077}, {644399, 640000, 0x0C222CE6},
{638977, 640000, 0x3CB3F547}, {630783, 640000, 0x926291B7},
{622593, 640000, 0x4BE31D76}, {614399, 640000, 0x87AD01DB},
{612113, 640000, 0xE29B49BB}, {602113, 640000, 0xE61272B7},
{580673, 720000, 0xC5E9DE8B}, {573441, 720000, 0xDC079BC0},
{565247, 720000, 0xCF1CA37C}, {557057, 720000, 0x9EEF945E},
{544767, 720000, 0xCC75A226}, {540673, 720000, 0x223549D1},
{532479, 720000, 0x40759687}, {524289, 720000, 0xA30037F1},
{522479, 720000, 0xAE25C4CA}, {516095, 720000, 0x2968525A},
{501041, 840000, 0x5D010F00}, {496943, 840000, 0x264D9BA7},
{487423, 840000, 0xE5FE5968}, {471041, 840000, 0x2A4CFB08},
{466943, 840000, 0x7CD3183C}, {458753, 840000, 0x84645EE0},
{450559, 840000, 0xE84CD133}, {442369, 840000, 0x930A5D84},
{441041, 840000, 0x7F778EED}, {436943, 840000, 0x31400F2C},
{420217, 1100000, 0x4D58EEF3}, {409601, 1100000, 0x4938363A},
{401407, 1100000, 0x92B347B5}, {393217, 1100000, 0xF6D354E3},
{392119, 1100000, 0x1D1D9D2E}, {389119, 1100000, 0x4DF62116},
{376833, 1100000, 0x4F526504}, {372735, 1100000, 0x3A3B365A},
{368641, 1100000, 0xBF818C14}, {360447, 1100000, 0xFAEF41BB},
{339487, 1400000, 0x62266123}, {335393, 1400000, 0x9198809B},
{331681, 1400000, 0x093642F5}, {329727, 1400000, 0xE092ED88},
{327681, 1400000, 0xD127F6AF}, {319487, 1400000, 0x7EDD49B9},
{315393, 1400000, 0x2AD1CBBB}, {311295, 1400000, 0xB501E32F},
{308295, 1400000, 0x58F6B52C}, {307201, 1400000, 0x382936EE},
{291913, 1500000, 0x34AC1486}, {286719, 1500000, 0x32151B08},
{282625, 1500000, 0x98F655CC}, {280335, 1500000, 0x47FF5C70},
{278527, 1500000, 0xF74DF4BE}, {274335, 1500000, 0x2322F4FA},
{270335, 1500000, 0xC065C6F4}, {266241, 1500000, 0x120A64F0},
{262143, 1500000, 0x7A473DE6}, {260335, 1500000, 0xB69E9EB9},
{250519, 1800000, 0x763B1556}, {245759, 1800000, 0xFBB67721},
{245281, 1800000, 0xF640633D}, {243713, 1800000, 0xCDC2C7AA},
{235519, 1800000, 0xC4A7AD0F}, {233473, 1800000, 0x39EF35D2},
{231183, 1800000, 0xB8792E3B}, {229375, 1800000, 0xE028677D},
{225281, 1800000, 0xFC11CE76}, {221183, 1800000, 0xACCF7139},
{212991, 2200000, 0x161FB56E}, {210415, 2200000, 0x7B60E81C},
{208897, 2200000, 0x63514A8F}, {204799, 2200000, 0xB1925D4B},
{200705, 2200000, 0x91E5EF6D}, {196607, 2200000, 0x0B2FA06D},
{194561, 2200000, 0x004E1A6D}, {188415, 2200000, 0x7C10EA53},
{186369, 2200000, 0xE723EC59}, {184319, 2200000, 0x1EC9F330},
{172031, 3200000, 0xA8289A03}, {163839, 3200000, 0x9BCEAD72},
{159745, 3200000, 0x4D30796D}, {157695, 3200000, 0x2719836B},
{155649, 3200000, 0x7C4B1002}, {153599, 3200000, 0x10F2B05E},
{147455, 3200000, 0x3BD06944}, {143361, 3200000, 0xA5C7C148},
{141311, 3200000, 0x71A19953}, {138527, 4000000, 0xD2E65D57},
{136241, 4000000, 0x99EE467C}, {135169, 4000000, 0x1115D06F},
{134335, 4000000, 0x32AA5A36}, {132143, 4000000, 0x392D9060},
{130335, 4000000, 0x689E07C6}, {130331, 4000000, 0xA791824A},
{125759, 5000000, 0x68E60664}, {125281, 5000000, 0x99421692},
{123713, 5000000, 0x883AC578}, {120519, 5000000, 0x6915D35E},
{119375, 5000000, 0x5930769A}, {115519, 5000000, 0x4092717B},
{115281, 5000000, 0x9C03F336}, {113473, 5000000, 0x15571C02},
{111183, 5000000, 0xAE6E91FF}, {111181, 5000000, 0x0E250D4F},
{108897, 6000000, 0xBEE96B52}, {104799, 6000000, 0xFF4FDA4D},
{102991, 6000000, 0x272CB267}, {100705, 6000000, 0xC0D285CF},
{100415, 6000000, 0x8FC75796}, {98415, 6000000, 0x55F0423B},
{96607, 6000000, 0xF60BA9EB}, {96369, 6000000, 0x5015EFE2},
{94561, 6000000, 0x27F5F9D8}, {94319, 6000000, 0x22FEEB22},
{83839, 7000000, 0xF3816D12}, {82031, 7000000, 0xD102B9B5},
{79745, 7000000, 0xDA483FC0}, {77695, 7000000, 0x62E51145},
{77455, 7000000, 0x9AEBD3EA}, {75649, 7000000, 0x64961C9D},
{73599, 7000000, 0x16415370}, {73361, 7000000, 0x87ED3BB9},
{71311, 7000000, 0x8F9E1C81}, {68527, 7000000, 0x44F5B375},
{66241, 8000000, 0x58E92942}, {65759, 8000000, 0x45F7CAD9},
{65281, 8000000, 0x71D13735}, {65169, 8000000, 0x9291C45D},
{64335, 8000000, 0x179EEB42}, {63713, 8000000, 0xC4D70CD3},
{62143, 8000000, 0x4EADFEDC}, {60519, 9000000, 0x59187C4E},
{60337, 8000000, 0x06B2F274}, {60335, 8000000, 0xF4A5C109},
{59375, 9000000, 0xECACBD29}, {58897, 9000000, 0x2D49F445},
{55519, 9000000, 0xCC57A689}, {55281, 9000000, 0x370811FF},
{54799, 11000000, 0xDF09CED1}, {53473, 11000000, 0x7527A443},
{52991, 11000000, 0x3F3E6D08}, {51183, 11000000, 0x63B7BC14},
{51181, 11000000, 0xCF23C3FE}, {50705, 11000000, 0xD7755CCA},
{50415, 11000000, 0xF13B5703}, {48415, 11000000, 0x18720A74},
{46607, 11000000, 0xEFAD69EB}, {46369, 11000000, 0x36576FF2},
{44561, 11000000, 0xBBFF519A}, {44319, 11000000, 0x67D8C7C8},
{43839, 14000000, 0xAB4287EC}, {42031, 14000000, 0x07E5F336},
{39745, 14000000, 0xB1F4CDA4}, {37695, 14000000, 0x5E68A976},
{37455, 14000000, 0x160940DF}, {35649, 14000000, 0x82C7BF50},
{35169, 14000000, 0xA9AA21D4}, {34527, 14000000, 0x746D4F98},
{33599, 14000000, 0xEEC3F4A4}, {33361, 14000000, 0x8060C92F},
{33241, 16000000, 0x6B01C7DF}, {32759, 16000000, 0x114A953B},
{32335, 16000000, 0x3E6C186B}, {32281, 16000000, 0x19CCFAF9},
{31713, 16000000, 0x8AFEC931}, {31311, 16000000, 0x7BF36CD8},
{31143, 16000000, 0x311C5B29}, {30519, 16000000, 0xD6403088},
{30335, 16000000, 0xCC66B636}, {30331, 16000000, 0x06615CC4},
{29897, 18000000, 0x376F4617}, {29375, 18000000, 0x2F7EEC43},
{27799, 18000000, 0xB887EB2B}, {27519, 18000000, 0x3E2FC829},
{27281, 18000000, 0x61D01456}, {26991, 18000000, 0x5500B456},
{26473, 22000000, 0x18B413C8}, {25705, 22000000, 0xE3A124A8},
{25415, 22000000, 0x5BAB711C}, {25183, 22000000, 0x4D02C76D},
{25181, 22000000, 0x54240561}, {24415, 22000000, 0x76E207FC},
{23607, 22000000, 0xE0ED69CD}, {23369, 22000000, 0x7B6B1955},
{22561, 22000000, 0xF68FF31A}, {22319, 22000000, 0x65736E87},
{21839, 28000000, 0x04ECD2F5}, {21031, 28000000, 0xBD7BB022},
{19745, 28000000, 0xC0E93F7C}, {18695, 28000000, 0x0E424A5F},
{18455, 28000000, 0xC0C87A73}, {17649, 28000000, 0xECB5F4E2},
{16599, 28000000, 0x2614F881}, {16361, 28000000, 0x9575CC6F},
{15311, 28000000, 0xDB715E07}, {15169, 28000000, 0xB7945996}
};

int selfTestInternal (
	int	thread_num,
	struct PriorityInfo *sp_info,
	unsigned long fftlen,
	unsigned int test_time,	/* Number of minutes to self-test */
	int	*torture_index,	/* Index into self test data array */
	unsigned int memory,	/* MB of memory the torture test can use */
	void	*bigbuf,	/* Memory block for the torture test */
	const struct self_test_info *test_data, /* Self test data */
	unsigned int test_data_count,
	int	*completed,	/* Returned count of tests completed */
	int	*errors,	/* Returned count of self test errors */
	int	*warnings)	/* Returned count of self test warnings */
{
	llhandle lldata;
	unsigned long k, limit, num_threads;
	unsigned int i, iter;
	char	buf[120];
//	char	iniName[32];
	time_t	start_time, current_time;
	int	stop_reason;

/* Set the title */

	title (thread_num, "Self-Test");

/* Decide how many threads the torture test can use.  This should only */
/* really be needed for QA purposes as the user can probably create more */
/* more stress by running one torture test window for each CPU core. */

	num_threads = IniGetInt (INI_FILE, "TortureTestThreads", 1);

/* Determine the range from which we'll choose an exponent to test. */

	limit = gwmap_fftlen_to_max_exponent (fftlen);

/* Get the current time */

	time (&start_time);

/* Start in the self test data array where we left off the last time */
/* torture test executed this FFT length. */

	i = (torture_index == NULL) ? 0 : *torture_index;

/* Loop testing various exponents from self test data array until */
/* time runs out */

	for (iter = 1; ; iter++) {
		char	fft_desc[100];
		unsigned long p, reshi, reslo;
		unsigned int ll_iters, num_gwnums;
		gwnum	*gwarray, g;

/* Find next self test data entry to work on */

		for ( ; ; i++) {

/* Wrap in the self test data array */

			if (i == test_data_count) i = 0;

/* Now select the actual exponent */

			p = test_data[i].p;
			if (p > limit) continue;

/* The SSE2 carry propagation code gets into trouble if there are too */
/* few bits per FFT word!  Thus, we'll require at least 8 bits per */
/* word here.  Now that the number of iterations changes for each FFT */
/* length I'm raising the requirement to 10 bits to keep timings roughly */
/* equal. */

			if (p / fftlen < 10) continue;

/* We've found an exponent to test! */

			break;
		}

/* Now run Lucas setup, for extra safety double the maximum allowable */
/* sum(inputs) vs. sum(outputs) difference.  For faster detection of unstable */
/* systems, enable SUM(INPUTS) != SUM(OUTPUTS) checking on the first test. */
/* For a better variety of tests, enable SUM(INPUTS) != SUM(OUTPUTS) checking half the time. */

		gwinit (&lldata.gwdata);
		gwclear_use_benchmarks (&lldata.gwdata);
		gwset_sum_inputs_checking (&lldata.gwdata, iter & 1);
		gwset_num_threads (&lldata.gwdata, num_threads);
		gwset_thread_callback (&lldata.gwdata, SetAuxThreadPriority);
		gwset_thread_callback_data (&lldata.gwdata, &sp_info);
		lldata.gwdata.GW_BIGBUF = (char *) bigbuf;
		lldata.gwdata.GW_BIGBUF_SIZE = (bigbuf != NULL) ? (size_t) memory * (size_t) 1048576 : 0;
		stop_reason = lucasSetup (thread_num, p, fftlen, &lldata);
		if (stop_reason) return (stop_reason);
		lldata.gwdata.MAXDIFF *= 2.0;

/* Output start message */

		ll_iters = test_data[i].iters;
		gwfft_description (&lldata.gwdata, fft_desc);
		sprintf (buf, SELF1, iter, ll_iters, p, fft_desc);
		OutputStr (thread_num, buf);

/* Determine how many gwnums we can allocate in the memory we are given */

		if (memory <= 8 || (iter & 1) == 0)
			num_gwnums = 1;
		else {
			num_gwnums = cvt_mem_to_gwnums (&lldata.gwdata, memory);
			if (num_gwnums < 1) num_gwnums = 1;
			if (num_gwnums > ll_iters) num_gwnums = ll_iters;
		}

/* Allocate gwnums to eat up the available memory */

		gwarray = (gwnum *) malloc (num_gwnums * sizeof (gwnum));
		if (gwarray == NULL) {
			lucasDone (&lldata);
			return (OutOfMemory (thread_num));
		}
		gwarray[0] = lldata.lldata;
		for (k = 1; k < num_gwnums; k++) {
			gwarray[k] = gwalloc (&lldata.gwdata);
			if (gwarray[k] == NULL) {
				num_gwnums = k;
				break;
			}
		}

/* Init data area with a pre-determined value */

restart_test:	dbltogw (&lldata.gwdata, 4.0, lldata.lldata);
		g = lldata.lldata;

/* Do Lucas-Lehmer iterations */

		for (k = 0; k < ll_iters; k++) {

/* Copy previous squared value (so we plow through memory) */

			if (k && num_gwnums > 1) {
				gwnum	prev;
				prev = g;
				g = gwarray[k % num_gwnums];
				gwcopy (&lldata.gwdata, prev, g);
			}

/* One Lucas-Lehmer test with error checking */

			gwsetnormroutine (&lldata.gwdata, 0, 1, 0);
			gwstartnextfft (&lldata.gwdata, k != ll_iters - 1);
			lucas_fixup (&lldata, p);
			gwsquare (&lldata.gwdata, g);

/* If the sum of the output values is an error (such as infinity) */
/* then raise an error. */

			if (gw_test_illegal_sumout (&lldata.gwdata)) {
				OutputBoth (thread_num, SELFFAIL1);
				(*warnings)++;
				if (*warnings < 100) {
					OutputBoth (thread_num, SELFFAIL4);
					goto restart_test;
				} else {
					OutputBoth (thread_num, SELFFAIL6);
					lucasDone (&lldata);
					free (gwarray);
					return (STOP_FATAL_ERROR);
				}
			}

/* Check that the sum of the input numbers squared is approximately */
/* equal to the sum of unfft results. */

			if (gw_test_mismatched_sums (&lldata.gwdata)) {
				sprintf (buf, SELFFAIL2,
					 gwsumout (&lldata.gwdata, g),
					 gwsuminp (&lldata.gwdata, g));
				OutputBoth (thread_num, buf);
				OutputBoth (thread_num, SELFFAIL5);
				(*errors)++;
				lucasDone (&lldata);
				free (gwarray);
				return (STOP_FATAL_ERROR);
			}

/* Make sure round off error is tolerable */

			if (gw_get_maxerr (&lldata.gwdata) > 0.45) {
				sprintf (buf, SELFFAIL3, gw_get_maxerr (&lldata.gwdata));
				OutputBoth (thread_num, buf);
				OutputBoth (thread_num, SELFFAIL5);
				(*errors)++;
				lucasDone (&lldata);
				free (gwarray);
				return (STOP_FATAL_ERROR);
			}

/* Abort if user demands it */

			stop_reason = stopCheck (thread_num);
			if (stop_reason) {
				lucasDone (&lldata);
				free (gwarray);
				return (stop_reason);
			}
		}

/* Copy the final result back to lldata.  If more than one gwnum was used */
/* then free the extra gwnums so that generateResidue64 has some memory */
/* to work with. */

		if (g != lldata.lldata)
			gwcopy (&lldata.gwdata, g, lldata.lldata);
		for (k = 1; k < num_gwnums; k++) {
			if (gwarray[k] != lldata.lldata)
				gwfree (&lldata.gwdata, gwarray[k]);
		}

/* Compare final 32 bits with the pre-computed array of correct residues */

		generateResidue64 (&lldata, p, &reshi, &reslo);
		lucasDone (&lldata);
		free (gwarray);
		(*completed)++;
		if (reshi != test_data[i].reshi) {
			sprintf (buf, SELFFAIL, reshi, test_data[i].reshi);
			OutputBoth (thread_num, buf);
			OutputBoth (thread_num, SELFFAIL5);
			(*errors)++;
			return (STOP_FATAL_ERROR);
		}

/* Bump index into self test data array */

		i++;

/* Has time expired? */

		time (&current_time);
		if ((unsigned int) (current_time - start_time) >= test_time * 60) break;
	}

/* Save our position in self test data array for next time torture test */
/* executes this FFT length */

	if (torture_index != NULL) *torture_index = i;

/* We've passed the self-test.  Remember this in the .INI file */
/* so that we do not need to do this again. */

	if (fftlen % 1024 == 0)
		sprintf (buf, SELFPASS, (int) (fftlen/1024), "K");
	else
		sprintf (buf, SELFPASS, (int) fftlen, "");
	OutputBoth (thread_num, buf);
//	sprintf (iniName, SelfTestIniMask, (int) (fftlen/1024));
//	IniWriteInt (LOCALINI_FILE, iniName, 1);
	return (0);
}

#ifdef ONE_HOUR_SELF_TEST

static const char SELFMSG1A[] = "The program will now perform a self-test to make sure the\n";
static const char SELFMSG1B[] = "Lucas-Lehmer code is working properly on your computer.\n";
static const char SELFMSG1C[] = "This will take about an hour.\n";

int selfTest (
	int	thread_num,
	struct PriorityInfo *sp_info,
	struct work_unit *w)
{
	unsigned long fftlen;
	char	iniName[32];
	int	tests_completed, self_test_errors, self_test_warnings;

/* What fft length are we running? */

	if (w->minimum_fftlen)
		fftlen = w->minimum_fftlen;
	else
		fftlen = gwmap_to_fftlen (1.0, 2, w->n, -1);

/* If fftlength is less than 64K return (we don't have any small exponents */
/* in our self test data) */

	if (fftlen < 65536) return (0);

/* Make sure we haven't run this self-test already. */

	sprintf (iniName, SelfTestIniMask, (int) (fftlen/1024));
	if (IniGetInt (LOCALINI_FILE, iniName, 0)) return (0);
#ifdef SERVER_TESTING
	return (0);
#endif

/* Make sure the user really wants to spend an hour doing this now */

	OutputStr (thread_num, SELFMSG1A);
	OutputStr (thread_num, SELFMSG1B);
	OutputStr (thread_num, SELFMSG1C);

/* Do the self test */

	tests_completed = 0;
	self_test_errors = 0;
	self_test_warnings = 0;
	return (selfTestInternal (thread_num, sp_info, fftlen, 60, NULL, 0, NULL,
				  &tests_completed, &self_test_errors, &self_test_warnings));
}
#endif

int tortureTest (
	int	thread_num,
	int	num_torture_workers)
{
	struct PriorityInfo sp_info;
	const struct self_test_info *test_data; /* Self test data */
	unsigned int test_data_count;
	int	num_lengths;		/* Number of FFT lengths we will torture test */
	unsigned long lengths[500];	/* The FFT lengths we will torture test */
	int	data_index[500];	/* Last exponent tested for each FFT length */
	int	test_time;
	int	tests_completed, self_test_errors, self_test_warnings;
	int	i, run_indefinitely, stop_reason;
	unsigned long fftlen, min_fft, max_fft;
	time_t	start_time, current_time;
	unsigned int memory;	/* Memory to use during torture test */
	void	*bigbuf = NULL;

/* Set the process/thread priority */

	sp_info.type = SET_PRIORITY_TORTURE;
	sp_info.worker_num = thread_num;
	sp_info.torture_num_workers = num_torture_workers;
	sp_info.torture_threads_per_test = IniGetInt (INI_FILE, "TortureTestThreads", 1);
	sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosityTorture", 0);
	sp_info.aux_thread_num = 0;
	SetPriority (&sp_info);

/* Init counters */

	tests_completed = 0;
	self_test_errors = 0;
	self_test_warnings = 0;

/* Pick which self test data array to use.  Machines are much faster now */
/* compared to when the torture test was introduced.  This new self test */
/* data will run more iterations and thus stress the cpu more by spending */
/* less time in the initialization code. */

	if (CPU_FLAGS & CPU_AVX) {
		test_data = SELF_TEST_DATA3;
		test_data_count = MAX_SELF_TEST_ITERS3;
	} else if (CPU_SPEED >= 1000.0) {
		test_data = SELF_TEST_DATA2;
		test_data_count = MAX_SELF_TEST_ITERS2;
	} else {
		test_data = SELF_TEST_DATA;
		test_data_count = MAX_SELF_TEST_ITERS;
	}

/* We used to support a menu option to run the self-test for an hour on */
/* each FFT length.  If we ever decide to resupport this option, change */
/* the run_indefinitely variable to an argument and change the output */
/* message below. */

loop:	run_indefinitely = TRUE;

/* Make sure the user really wants to spend many hours doing this now */

	if (run_indefinitely) {
		OutputStr (thread_num, TORTURE1);
		OutputStr (thread_num, TORTURE2);
		test_time = IniGetInt (INI_FILE, "TortureTime", 15);
	}

/* Determine fft lengths we should run and allocate a big block */
/* of memory to test. */

	min_fft = IniGetInt (INI_FILE, "MinTortureFFT", 8) * 1024;
	if (min_fft < 32) min_fft = 32;
	max_fft = IniGetInt (INI_FILE, "MaxTortureFFT", 4096) * 1024;
	memory = IniGetInt (INI_FILE, "TortureMem", 8);
	while (memory > 8 && bigbuf == NULL) {
		bigbuf = aligned_malloc ((size_t) memory * (size_t) 1048576, 128);
		if (bigbuf == NULL) memory--;
	}

/* Enumerate the FFT lengths we will torture test. */

	num_lengths = 0;
	fftlen = gwmap_to_fftlen (1.0, 2, 15 * min_fft, -1);
	while (fftlen <= max_fft) {
		unsigned long max_exponent = gwmap_fftlen_to_max_exponent (fftlen);
		if (fftlen >= min_fft && max_exponent > test_data[test_data_count-1].p) {
			lengths[num_lengths] = fftlen;
			data_index[num_lengths++] = 0;
		}
		fftlen = gwmap_to_fftlen (1.0, 2, max_exponent + 100, -1);
		if (fftlen == 0) break;
	}

/* Raise error if no FFT lengths to test */

	if (num_lengths == 0) {
		OutputStr (thread_num, "No FFT lengths available in the range specified.\n");
		return (0);
	}

/* For historical reasons, we alternate testing big and small FFT lengths */
/* (the theory being we'll find bad memory or an overheat problem more quickly). */

	for (i = 0; i <= num_lengths / 2 - 2; i += 2) {
		int	temp;
		temp = lengths[i];
		lengths[i] = lengths[i + num_lengths / 2];
		lengths[i + num_lengths / 2] = lengths[i + num_lengths / 2 + 1];
		lengths[i + num_lengths / 2 + 1] = lengths[i + 1];
		lengths[i + 1] = temp;
	}

/* Now self-test each fft length */

	stop_reason = 0;
	time (&start_time);
	for ( ; ; ) {
	    for (i = 0; i < num_lengths; i++) {
		stop_reason =
			selfTestInternal (thread_num, &sp_info, lengths[i], test_time, &data_index[i],
					  memory, bigbuf, test_data, test_data_count, &tests_completed,
					  &self_test_errors, &self_test_warnings);
		if (stop_reason) {
			char	buf[120];
			int	hours, minutes;
			time (&current_time);
			minutes = (int) (current_time - start_time) / 60;
			hours = minutes / 60;
			minutes = minutes % 60;
			sprintf (buf, "Torture Test completed %d tests in ", tests_completed);
			if (hours > 1) sprintf (buf+strlen(buf), "%d hours, ", hours);
			else if (hours == 1) sprintf (buf+strlen(buf), "1 hour, ");
			sprintf (buf+strlen(buf),
				 "%d minutes - %d errors, %d warnings.\n",
				 minutes, self_test_errors, self_test_warnings);
			OutputStr (thread_num, buf);
			run_indefinitely = FALSE;
			break;
		}
	    }
	    if (! run_indefinitely) break;
	}

/* Self test completed!  Free memory. */

	aligned_free (bigbuf);

/* If this was a user requested stop, then wait for a restart */

	while (stop_reason == STOP_WORKER) {
		implement_stop_one_worker (thread_num);
		stop_reason = stopCheck (thread_num);
		if (stop_reason == 0) goto loop;
	}

/* All done */

	return (stop_reason);
}

/*******************************************/
/* Various QA and data analysis functions! */
/*******************************************/

/* Read a file of exponents to run LL iterations on as part of a QA process */
/* The format of this file is: */
/*	exponent,optional fft length,num iters,optional shift count,residue */
/* An Advanced/Time 9999 corresponds to type 0, Advanced/Time 9998 */
/* corresponds to type 1, etc. */
/* Type 0 executes much like an LL test, error checking and doing a */
/* careful iteration occasionally */
/* Type 1 does roundoff checking every iteration and accumulates */
/* statistics on the round off data. */
/* Type 2 and higher have not been used much and may not work */

int lucas_QA (
	int	thread_num,
	int	type)
{
	llhandle lldata;
	FILE	*fd;
	int	stop_reason;

/* Set the title, init random generator */

	title (thread_num, "QA");
	srand ((unsigned) time (NULL));

/* Open QA file */

	fd = fopen ("qa", "r");
	if (fd == NULL) {
		OutputStr (thread_num, "File named 'qa' could not be opened.\n");
		return (STOP_FILE_IO_ERROR);
	}

/* Loop until the entire file is processed */

	for ( ; ; ) {
		unsigned long p, p_limit, fftlen, iters;
		char	buf[500], res[80];
		unsigned long reshi, reslo, units_bit;
		unsigned long i, maxerrcnt;
		double	maxsumdiff, maxerr, toterr, M, S;
		unsigned long ge_300, ge_325, ge_350, ge_375, ge_400;
		gwnum	t1, t2;
		unsigned int iters_unchecked, M_count;

/* Read a line from the file */

		p = 0;
		(void) fscanf (fd, "%lu,%lu,%lu,%lu,%s\n", &p, &fftlen, &iters, &units_bit, res);
		if (p == 0) break;

		maxsumdiff = 0.0;
		ge_300 = ge_325 = ge_350 = ge_375 = ge_400 = 0;
		maxerr = 0.0; maxerrcnt = 0; toterr = 0.0;
		iters_unchecked = (type > 3) ? 2 : 40;
		M = 0.0;  S = 0.0;  M_count = 0;

/* Now run Lucas setup */

		if (type == 4) iters /= 10, p_limit = p - 20;
		else p_limit = p;
		for ( ; p >= p_limit; p -= 2) {

		gwinit (&lldata.gwdata);
		gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
		stop_reason = lucasSetup (thread_num, p, fftlen, &lldata);
		if (stop_reason) { stop_reason = 0; goto not_impl; }
		lldata.units_bit = units_bit;

/* Check for a randomized units bit */

		if (lldata.units_bit >= p) {
			uint32_t hi, lo;
			lldata.units_bit = (rand () << 16) + rand ();
			if (CPU_FLAGS & CPU_RDTSC) {
				rdtsc (&hi, &lo);
				lldata.units_bit += lo;
			}
			lldata.units_bit = lldata.units_bit % p;
		}

/* Init data area with a pre-determined value */

		if (type == 3 || type == 4)
			gw_random_number (&lldata.gwdata, lldata.lldata);
		else {
			unsigned long word, bit_in_word;
			bitaddr (&lldata.gwdata, (lldata.units_bit + 2) % p, &word, &bit_in_word);
			for (i = 0; i < gwfftlen (&lldata.gwdata); i++)
				set_fft_value (&lldata.gwdata, lldata.lldata, i, (i == word) ? (1L << bit_in_word) : 0);
		}

/* The thorough, P-1, and ECM tests use more than one number */

		if (type == 2 || type == 3) {
			t1 = gwalloc (&lldata.gwdata);
			dbltogw (&lldata.gwdata, 234872639921.0, t1);
			gwfft (&lldata.gwdata, t1, t1);
			t2 = gwalloc (&lldata.gwdata);
			dbltogw (&lldata.gwdata, 1982387192367.0, t2);
			gwfft (&lldata.gwdata, t2, t2);
			lldata.gwdata.MAXDIFF *= 16;
		}

/* Do Lucas-Lehmer iterations */

		for (i = 0; i < iters; i++) {

/* One Lucas-Lehmer iteration with error checking */

			if (type == 0) {		/* Typical LL test */
				gwsetnormroutine (&lldata.gwdata, 0, (i & 63) == 37, 0);
				gwstartnextfft (&lldata.gwdata, i < iters / 2);
				lucas_fixup (&lldata, p);
				gwsquare (&lldata.gwdata, lldata.lldata);
			} else if (type == 1 || type == 4) { /* Gather stats */
				gwsetnormroutine (&lldata.gwdata, 0, 1, 0);
				gwstartnextfft (&lldata.gwdata, i < iters / 2);
				lucas_fixup (&lldata, p);
				gwsquare (&lldata.gwdata, lldata.lldata);
			} else if (type == 2) {		/* Thorough test */
				unsigned long j;
				for (j = 0; j < (i & 7); j++) {
					gwadd (&lldata.gwdata, lldata.lldata, lldata.lldata);
					lldata.units_bit = (lldata.units_bit+1) % p;
				}
				if ((i & 15) == 13) {
					gwadd3quick (&lldata.gwdata, lldata.lldata, lldata.lldata, t1);
					gwsub3quick (&lldata.gwdata, t1, lldata.lldata, lldata.lldata);
					gwadd3 (&lldata.gwdata, lldata.lldata, lldata.lldata, t1);
					gwsub3 (&lldata.gwdata, t1, lldata.lldata, lldata.lldata);
					gwaddsub4 (&lldata.gwdata, lldata.lldata, lldata.lldata, t1, t2);
					gwaddsub (&lldata.gwdata, t1, lldata.lldata);
					gwadd (&lldata.gwdata, t2, lldata.lldata);
				}
				lucas_fixup (&lldata, p);
				if ((i & 3) == 0) {
					gwsquare (&lldata.gwdata, lldata.lldata);
				} else if ((i & 3) == 1) {
					gwfft (&lldata.gwdata, lldata.lldata, lldata.lldata);
					gwfftfftmul (&lldata.gwdata, lldata.lldata, lldata.lldata, lldata.lldata);
				} else {
					gwfft (&lldata.gwdata, lldata.lldata, t1);
					gwfftmul (&lldata.gwdata, t1, lldata.lldata);
				}
			} else if (type == 3) {		/* Typical ECM run */
				lucas_fixup (&lldata, p);
				gwfftsub3 (&lldata.gwdata, t1, t2, t2);
				gwfft (&lldata.gwdata, lldata.lldata, lldata.lldata);
				gwfftfftmul (&lldata.gwdata, t2, lldata.lldata, t2);
				gwswap (t1, lldata.lldata);
				gwswap (t2, lldata.lldata);
			}

/* Keep track of the standard deviation - see Knuth vol 2 */

			if (i > iters_unchecked) {
				double	newM;
				toterr += gw_get_maxerr (&lldata.gwdata);
				M_count++;
				newM = M + (gw_get_maxerr (&lldata.gwdata) - M) / M_count;
				S = S + (gw_get_maxerr (&lldata.gwdata) - M) * (gw_get_maxerr (&lldata.gwdata) - newM);
				M = newM;

/* Maintain range info */

				if (gw_get_maxerr (&lldata.gwdata) >= 0.300) ge_300++;
				if (gw_get_maxerr (&lldata.gwdata) >= 0.325) ge_325++;
				if (gw_get_maxerr (&lldata.gwdata) >= 0.350) ge_350++;
				if (gw_get_maxerr (&lldata.gwdata) >= 0.375) ge_375++;
				if (gw_get_maxerr (&lldata.gwdata) >= 0.400) ge_400++;

/* Maintain maximum error info */

				if (gw_get_maxerr (&lldata.gwdata) > maxerr) maxerr = gw_get_maxerr (&lldata.gwdata), maxerrcnt = 1;
				else if (gw_get_maxerr (&lldata.gwdata) == maxerr) maxerrcnt++;
			}
			gw_clear_maxerr (&lldata.gwdata);

/* Maintain maximum suminp/sumout difference */

			if (fabs (gwsuminp (&lldata.gwdata, lldata.lldata) -
				  gwsumout (&lldata.gwdata, lldata.lldata)) > maxsumdiff) {
				maxsumdiff = fabs (gwsuminp (&lldata.gwdata, lldata.lldata) -
						   gwsumout (&lldata.gwdata, lldata.lldata));
			}

/* If the sum of the output values is an error (such as infinity) */
/* then raise an error.  For some reason these bad values are treated */
/* as zero by the C compiler.  There is probably a better way to */
/* check for this error condition. */

			if (gw_test_illegal_sumout (&lldata.gwdata)) {
				OutputBoth (thread_num, "Warning: ILLEGAL SUMOUT\n");
				dbltogw (&lldata.gwdata, 11.0, lldata.lldata);
				gw_clear_error (&lldata.gwdata);
			}

/* Check that the sum of the input numbers squared is approximately */
/* equal to the sum of unfft results. */

			if (gw_test_mismatched_sums (&lldata.gwdata)) {
				OutputBoth (thread_num, "Warning: SUMOUT MISMATCH\n");
				gw_clear_error (&lldata.gwdata);
			}

/* Abort if user demands it */

			stop_reason = stopCheck (thread_num);
			if (stop_reason) {
				lucasDone (&lldata);
				fclose (fd);
				return (stop_reason);
			}
		}

/* Generate residue and cleanup */

		generateResidue64 (&lldata, p, &reshi, &reslo);
		lucasDone (&lldata);
		}

		if (type == 4) iters *= 10, iters_unchecked *= 10, p = p_limit + 20;
		else p = p_limit;

/* Output array of distributions of MAXERR */

		if (type == 1 || type == 3 || type == 4) {
			S = sqrt (S / (M_count - 1));
			toterr /= M_count;
			sprintf (buf, "avg: %6.6f, stddev: %6.6f, #stdev to 0.5: %6.6f\n",
				 toterr, S, (0.50 - toterr) / S);
			OutputBoth (thread_num, buf);
		}

/* Compare residue with correct residue from the input file */

		sprintf (buf, "%08lX%08lX", reshi, reslo);
		if (type <= 2 && _stricmp (res, buf)) {
			sprintf (buf, "Warning: Residue mismatch. Expected %s\n", res);
			OutputBoth (thread_num, buf);
		}

/* Output message */

		sprintf (buf, "Exp/iters: %lu/%lu, res: %08lX%08lX, maxerr: %6.6f/%lu, %lu/%lu/%lu/%lu/%lu, maxdiff: %9.9f/%9.9f\n",
			 p, iters, reshi, reslo, maxerr, maxerrcnt,
			 ge_300, ge_325, ge_350, ge_375, ge_400,
			 maxsumdiff, lldata.gwdata.MAXDIFF);
		OutputBoth (thread_num, buf);
not_impl:	;
	}
	fclose (fd);

	return (0);
}

/* Test the factoring program */

int primeSieveTest (
	int	thread_num)
{
	fachandle facdata;
	char	buf[500];
	FILE	*fd;
	unsigned long p;
	int	stop_reason;
	uint32_t res, carryl, carryh;

/* Open factors file */

	fd = fopen ("factors", "r");
	if (fd == NULL) {
		OutputBoth (thread_num, "File named 'factors' not found.\n");
		return (0);
	}

/* Loop until all the entire range is factored */

	while (fscanf (fd, "%ld", &p) && p) {
		unsigned long fachi, facmid, faclo;
		unsigned long i, pass;
		char fac[480];
		char *f;

/* What is the factor? */

		(void) fscanf (fd, "%s", fac);
		fachi = facmid = faclo = 0;
		for (f = fac; *f; f++) {
			if (*f < '0' || *f > '9') continue;
			res = *f - '0';
			carryl = 0;
			muladdhlp (&res, &carryl, &carryh, faclo, 10);
			faclo = res;
			res = carryl;
			carryl = 0;
			muladdhlp (&res, &carryl, &carryh, facmid, 10);
			facmid = res;
			fachi = fachi * 10 + carryl;
			if (fachi >= 268435456 ||
			    (fachi >= 4194304 && !(CPU_FLAGS & CPU_FMA3)) ||
			    (fachi >= 16384 && !(CPU_FLAGS & CPU_SSE2))) {
				sprintf (buf, "%ld %s factor too big.\n", p, fac);
				OutputBoth (thread_num, buf);
				goto nextp;
			}
		}

/* See if p is too small (less than 2^44) */

		if (fachi == 0 && facmid < 0x1000) {
			sprintf (buf, "%ld %s factor too small.\n", p, fac);
			OutputBoth (thread_num, buf);
			goto nextp;
		}

/* See if p is a prime */

		if (! isPrime (p)) {
			sprintf (buf, "%ld not a prime.\n", p);
			OutputBoth (thread_num, buf);
			goto nextp;
		}

/* Setup the factoring program */

		i = (fachi % 120 * 16 + facmid % 120 * 16 + faclo % 120) % 120;
		if (i == 1) pass = 0;
		else if (i == 7) pass = 1;
		else if (i == 17) pass = 2;
		else if (i == 23) pass = 3;
		else if (i == 31) pass = 4;
		else if (i == 41) pass = 5;
		else if (i == 47) pass = 6;
		else if (i == 49) pass = 7;
		else if (i == 71) pass = 8;
		else if (i == 73) pass = 9;
		else if (i == 79) pass = 10;
		else if (i == 89) pass = 11;
		else if (i == 97) pass = 12;
		else if (i == 103) pass = 13;
		else if (i == 113) pass = 14;
		else if (i == 119) pass = 15;
		else goto bad;
		if (IniGetInt (INI_FILE, "QAMultithreadedTF", 0))
			facdata.num_threads = (facmid & 0x7) + 1;
		else
			facdata.num_threads = 1;
		stop_reason = factorSetup (thread_num, p, &facdata);
		if (stop_reason) {
			fclose (fd);
			return (stop_reason);
		}
		if (fachi >= 16384 && !(facdata.asm_data->cpu_flags & (CPU_SSE2 | CPU_AVX2 | CPU_FMA3 | CPU_AVX512F))) {
			sprintf (buf, "%ld %s factor too big.\n", p, fac);
			OutputBoth (thread_num, buf);
			factorDone (&facdata);
			goto nextp;
		}
		/* Set endpoint to next power of two or, to make QA with more than one siever faster, */
		/* set endpoint to 10% higher than the factor to find. */
		{
			double	fltfac;
			fltfac = ((double) fachi * 4294967296.0 + (double) facmid) * 4294967296.0 + (double) faclo;
			facdata.endpt = pow (2.0, ceil (_log2 (fltfac)));
			if (facdata.endpt > fltfac * 1.1) facdata.endpt = fltfac * 1.1;
		}
		facdata.asm_data->FACHSW = fachi;
		facdata.asm_data->FACMSW = facmid & ~0xFFF;
		stop_reason = factorPassSetup (thread_num, pass, &facdata);
		if (stop_reason) {
			fclose (fd);
			factorDone (&facdata);
			return (stop_reason);
		}

/* Factor found, is it a match? */

		do {
			if (factorChunk (&facdata) != 2) {
				if (facdata.asm_data->FACHSW == fachi &&
				    facdata.asm_data->FACMSW == facmid &&
				    facdata.asm_data->FACLSW == faclo) {
					sprintf (buf, "%ld %s factored OK.\n", p, fac);
				} else {
					sprintf (buf, "%ld %s different factor found.\n", p, fac);
				}
				OutputSomewhere (thread_num, buf);
				factorDone (&facdata);
				goto nextp;
			}
#ifdef X86_64
		} while (facdata.total_num_chunks_TFed != facdata.total_num_chunks_to_TF && facdata.total_num_chunks_TFed < 200000);
#else
		} while (facdata.asm_data->FACMSW <= facmid);
#endif

/* Uh oh. */

bad:		sprintf (buf, "%ld %s factor not found.\n", p, fac);
		OutputBoth (thread_num, buf);

/* If an escape key was hit, write out the results and return */

		factorDone (&facdata);
nextp:		stop_reason = stopCheck (thread_num);
		if (stop_reason) {
			fclose (fd);
			return (stop_reason);
		}
		p = 0;
	}

/* All done */

	fclose (fd);
	return (0);
}

/******************/
/* Debugging code */
/******************/

int cpuid_dump (
	int	thread_num)
{
	struct cpuid_data reg;
	unsigned int i, j, max_cpuid_value, max_extended_cpuid_value;
	char	buf[200];
#define dumpreg() {sprintf(buf,"i: %08lX, EAX: %08lX, EBX: %08lX, ECX: %08lX, EDX: %08lX\n",(long)i,(long)reg.EAX,(long)reg.EBX,(long)reg.ECX,(long)reg.EDX); OutputBoth(thread_num,buf);}

/* Call CPUID with 0 and 0x80000000 arguments to get how many functions are supported. */

	Cpuid (0, &reg);
	max_cpuid_value = reg.EAX;
	Cpuid (0x80000000, &reg);
	max_extended_cpuid_value = reg.EAX;

/* Dump the regular CPUID data */

	for (i = 0; i <= max_cpuid_value; i++) {
		for (j = 0; j < 5; j++) {
			memset (&reg, 0, sizeof (reg));
			reg.ECX = j;
			Cpuid (i, &reg);
			dumpreg ();
			if (i != 4 && i != 7 && i != 11) break;
		}
	}

/* Dump the extended CPUID data */

	for (i = 0x80000000; i <= max_extended_cpuid_value; i++) {
		for (j = 0; j < 5; j++) {
			memset (&reg, 0, sizeof (reg));
			reg.ECX = j;
			Cpuid (i, &reg);
			dumpreg ();
			if (i != 0x8000001D) break;
		}
	}

/* Dump the extended GETBV (control registers) data */

	Xgetbv (0, &reg);
	dumpreg ();
	if (IniGetInt (INI_FILE, "AdditionalGetBV", 0)) {
		Xgetbv (IniGetInt (INI_FILE, "AdditionalGetBV", 0), &reg);
		dumpreg ();
	}

	return (0);
}

/*********************/
/* Benchmarking code */
/*********************/

/* Time a few iterations of an LL test on a given exponent */

int primeTime (
	int	thread_num,
	unsigned long p,
	unsigned long iterations)
{
	struct PriorityInfo sp_info;
#define SAVED_LIMIT	10
	llhandle lldata;
	unsigned long i, j, saved, save_limit;
	int	min_cores, max_cores, incr_cores, min_hyperthreads, max_hyperthreads;
	int	num_cores, num_hyperthreads;
	char	buf[120], fft_desc[100];
	double	time, saved_times[SAVED_LIMIT];
	int	days, hours, minutes, stop_reason, print_every_iter;
	uint32_t *ASM_TIMERS;
	uint32_t best_asm_timers[32] = {0};
	double	timers[2];

/* Look for special values to run QA suites */

	if (p >= 9900 && p <= 9999) {
		sp_info.type = SET_PRIORITY_QA;
		sp_info.worker_num = thread_num;
		sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosityQA", 0);
		sp_info.aux_thread_num = 0;
		SetPriority (&sp_info);

		if (p >= 9994 && p <= 9999)
			return (lucas_QA (thread_num, 9999 - p));
		if (p == 9992)
			return (pminus1_QA (thread_num, &sp_info));
		if (p == 9991)
			return (ecm_QA (thread_num, &sp_info));
		if (p == 9990)
			return (primeSieveTest (thread_num));
		if (p == 9950)
			return (cpuid_dump (thread_num));
		if (p >= 9900 && p <= 9919)
			return (test_randomly (thread_num, &sp_info));
		return (test_all_impl (thread_num, &sp_info));
	}

/* Set the process/thread priority */

	sp_info.type = SET_PRIORITY_TIME;
	sp_info.worker_num = thread_num;
	sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosityTime", 0);
	sp_info.aux_thread_num = 0;
	sp_info.time_hyperthreads = 1;
	SetPriority (&sp_info);

/* Get settings from INI file */

	min_cores = IniGetInt (INI_FILE, "AdvancedTimeMinCores", 1);
	if (min_cores < 1) min_cores = 1;
	if (min_cores > (int) NUM_CPUS) min_cores = NUM_CPUS;

	max_cores = IniGetInt (INI_FILE, "AdvancedTimeMaxCores", NUM_CPUS);
	if (max_cores < 1) max_cores = 1;
	if (max_cores > (int) NUM_CPUS) max_cores = NUM_CPUS;

	incr_cores = IniGetInt (INI_FILE, "AdvancedTimeCoresIncrement", 1);
	if (incr_cores < 1) incr_cores = 1;

	min_hyperthreads = IniGetInt (INI_FILE, "AdvancedTimeMinHyperthreads", 1);
	if (min_hyperthreads < 1) min_hyperthreads = 1;
	if (min_hyperthreads > (int) CPU_HYPERTHREADS) min_hyperthreads = CPU_HYPERTHREADS;

	max_hyperthreads = IniGetInt (INI_FILE, "AdvancedTimeMaxHyperthreads", CPU_HYPERTHREADS);
	if (max_hyperthreads < 1) max_hyperthreads = 1;
	if (max_hyperthreads > (int) CPU_HYPERTHREADS) max_hyperthreads = CPU_HYPERTHREADS;

	print_every_iter = IniGetInt (INI_FILE, "PrintTimedIterations", 1);

/* Loop through all possible cores and hyperthreads values */

	for (num_hyperthreads = min_hyperthreads; num_hyperthreads <= max_hyperthreads; num_hyperthreads++) {
		sp_info.time_hyperthreads = num_hyperthreads;
		for (num_cores = min_cores; num_cores <= max_cores; num_cores += incr_cores) {
			if (!OS_CAN_SET_AFFINITY && num_hyperthreads > 1 && num_cores != NUM_CPUS) continue;

/* Clear all timers */

			clear_timers (timers, sizeof (timers) / sizeof (timers[0]));

/* Init the FFT code */

			gwinit (&lldata.gwdata);
			gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
			gwset_bench_cores (&lldata.gwdata, NUM_CPUS);		  // We're most likely to have bench data for this case
			gwset_bench_workers (&lldata.gwdata, NUM_WORKER_THREADS); // We're most likely to have bench data for this case
			if (ERRCHK) gwset_will_error_check (&lldata.gwdata);
			else gwset_will_error_check_near_limit (&lldata.gwdata);
			if (IniGetInt (LOCALINI_FILE, "UseLargePages", 0))
				gwset_use_large_pages (&lldata.gwdata);
			// Here is a hack to let me time different FFT implementations.
			// For example, 39000001 times the first 2M FFT implementation,
			// 39000002 times the second 2M FFT implementation, etc.
			if (IniGetInt (INI_FILE, "TimeSpecificFFTImplementations", 0))
				lldata.gwdata.bench_pick_nth_fft = p % 100;
			gwset_num_threads (&lldata.gwdata, num_cores * num_hyperthreads);
			gwset_thread_callback (&lldata.gwdata, SetAuxThreadPriority);
			gwset_thread_callback_data (&lldata.gwdata, &sp_info);
			stop_reason = lucasSetup (thread_num, p, IniGetInt (INI_FILE, "TimePlus1", 0), &lldata);
			if (stop_reason) return (stop_reason);
			ASM_TIMERS = get_asm_timers (&lldata.gwdata);
			memset (ASM_TIMERS, 0, 32 * sizeof (uint32_t));

/* Output a message about the FFT length */

			gwfft_description (&lldata.gwdata, fft_desc);
			sprintf (buf, "Using %s\n", fft_desc);
			OutputStr (thread_num, buf);
			title (thread_num, "Timing");

/* Fill data space with random values. */

			generateRandomData (&lldata);

/* Do one squaring untimed, to prime the caches and start the */
/* post-FFT process going. */

			gwsetnormroutine (&lldata.gwdata, 0, ERRCHK != 0, 0);
			gwstartnextfft (&lldata.gwdata, TRUE);
			gwsquare (&lldata.gwdata, lldata.lldata);

/* Compute numbers in the lucas series */
/* Note that for reasons unknown, we've seen cases where printing out */
/* the times on each iteration greatly impacts P4 timings. */

			save_limit = (p <= 4000000) ? SAVED_LIMIT : 1;
			for (i = 0, saved = 0; i < iterations; i++) {

/* Time a single squaring */

				start_timer (timers, 0);
				gwsquare (&lldata.gwdata, lldata.lldata);
				end_timer (timers, 0);
				timers[1] += timers[0];
				saved_times[saved++] = timers[0];
				timers[0] = 0;

/* Remember the best asm timers (used when I'm optimizing assembly code) */

				for (j = 0; j < 32; j++)
					if (i == 0 || ASM_TIMERS[j] < best_asm_timers[j])
						best_asm_timers[j] = ASM_TIMERS[j];

/* Output timer squaring times */

				if (saved == save_limit || i == iterations - 1) {
					if (print_every_iter)
						for (j = 0; j < saved; j++) {
							sprintf (buf, "p: %lu.  Time: ", p);
							timers[0] = saved_times[j];
							print_timer (timers, 0, buf, TIMER_MS | TIMER_NL | TIMER_CLR);
							OutputStr (thread_num, buf);
						}
					saved = 0;
				}

/* Abort early if so requested */

				stop_reason = stopCheck (thread_num);
				if (stop_reason) {
					lucasDone (&lldata);
					return (stop_reason);
				}
			}
			lucasDone (&lldata);
			time = timer_value (timers, 1);

/* Print an estimate for how long it would take to test this number */

			sprintf (buf, "Iterations: %lu.  Total time: ", iterations);
			print_timer (timers, 1, buf, TIMER_NL | TIMER_CLR);
			OutputStr (thread_num, buf);
			time = time * p / iterations;
			days = (int) (time / 86400.0); time -= (double) days * 86400.0;
			hours = (int) (time / 3600.0); time -= (double) hours * 3600.0;
			minutes = (int) (time / 60.0);
			strcpy (buf, "Estimated time to complete this exponent: ");
			sprintf (buf+strlen(buf), days == 1 ? "%d day, " : "%d days, ", days);
			sprintf (buf+strlen(buf), hours == 1 ? "%d hour, " : "%d hours, ", hours);
			sprintf (buf+strlen(buf), minutes == 1 ? "%d minute.\n" : "%d minutes.\n", minutes);
			OutputStr (thread_num, buf);

/* I use these assembly language timers to time various chunks of */
/* assembly code.  Print these timers out. */

			for (i = 0; i < 32; i++) {
				sprintf (buf, "timer %lu: %d\n", i, (int) best_asm_timers[i]);
				if (best_asm_timers[i]) OutputBoth (thread_num, buf);
			}

/* End loop through all possible cores and hyperthreads */

		}
	}

/* All done */

	return (0);
}

/* Busy loop to keep CPU cores occupied.  Used during */
/* a benchmark so that turbo boost does not kick in. */

int	last_bench_cpu_num = 0;

void bench_busy_loop (void *arg)
{
	int	cpu_num;
	struct PriorityInfo sp_info;

/* Only put newer CPUs into a busy loop.  We do this because one_hundred_thousand_clocks */
/* uses SSE2 instructions and pre-SSE2 machines don't have speed step / turbo boost. */
/* Without this check, dual-core Pentium 2 and 3 machines will crash benchmarking. */

	if (! (CPU_FLAGS & CPU_SSE2)) return;

/* Set the affinity so that busy loop runs on the specified CPU core */

	cpu_num = (int) (intptr_t) arg;
	sp_info.type = SET_PRIORITY_BUSY_LOOP;
	sp_info.worker_num = MAIN_THREAD_NUM;
	sp_info.busy_loop_cpu = cpu_num;
	sp_info.verbose_flag = 0;
	SetPriority (&sp_info);

/* Stay busy until last_bench_cpu_num says this CPU thread should close */

	while (cpu_num > last_bench_cpu_num) one_hundred_thousand_clocks ();
}

/* Routine to add a timing to benchmark packet sent to server */

void add_bench_data_to_pkt (
	struct primenetBenchmarkData *pkt,	/* Benchmarking packet to send to server */
	const char *bench_format_mask,		/* Sprintf format mask */
	unsigned long fftlen,			/* FFT length benchmarked */
	double numeric_value,			/* Numeric value to send */
	int   smaller_is_better)		/* TRUE if smaller numbers are considered "better" */
{
	char	bench_str[20];
	int	i;

/* Format the benchmark string */

	sprintf (bench_str, bench_format_mask, fftlen / 1024);

/* See if we already have a value for this benchmark string.  If so, replace the value if this one is better. */

	for (i = 0; i < (int) pkt->num_data_points; i++) {
		if (strcmp (pkt->data_points[i].bench, bench_str) == 0) {
			if ((smaller_is_better && numeric_value < pkt->data_points[i].timing) ||
			    (!smaller_is_better && numeric_value > pkt->data_points[i].timing))
				pkt->data_points[i].timing = numeric_value;
			return;
		}
	}

/* Append this item to the benchmark data */

	if (pkt->num_data_points < PRIMENET_BENCH_MAX_DATAPOINTS) {
		strcpy (pkt->data_points[i].bench, bench_str);
		pkt->data_points[pkt->num_data_points].timing = numeric_value;
		pkt->num_data_points++;
	}
}

/* Routine to benchmark the trial factoring code */

static const char BENCH1[] = "Your timings will be written to the results.txt file.\n";
static const char BENCH2[] = "Compare your results to other computers at http://www.mersenne.org/report_benchmarks\n";

int factorBench (
	int	thread_num)
{
	fachandle facdata;
	unsigned long num_lengths, i, j;
	double	best_time;
	char	buf[512];
	int	bit_lengths[] = {61, 62, 63, 64, 65, 66, 67, 75, 76, 77};
	int	res, stop_reason;
	double	timers[2];

/* Keep the other CPU cores busy.  This should prevent "turbo boost" from kicking in. */
/* We do this to hopefully produce more consistent benchmarks.  We don't want to report */
/* a CPU speed of 1.87 GHz and then produce a benchmark running at a boosted 3.2 GHz */
/* (this happens on a Core i7 Q840M processor). */

	if (IniGetInt (INI_FILE, "BenchDummyWorkers", 0)) {
		last_bench_cpu_num = 0;			// CPU #0 is benching
		for (i = 1; i < NUM_CPUS; i++) {	// CPU #1 to NUM_CPUS-1 are busy looping
			gwthread thread_id;
			gwthread_create (&thread_id, &bench_busy_loop, (void *) (intptr_t) i);
		}
	}

/* Loop over all trial factor lengths */

	num_lengths = sizeof (bit_lengths) / sizeof (int);
	for (i = 0; i < num_lengths; i++) {

/* Initialize for this bit length. */

		facdata.num_threads = 1;
		stop_reason = factorSetup (thread_num, 35000011, &facdata);
		if (stop_reason) {
			last_bench_cpu_num = NUM_CPUS;
			return (stop_reason);
		}
		if (bit_lengths[i] <= 64) {
			facdata.asm_data->FACHSW = 0;
			facdata.asm_data->FACMSW = 1L << (bit_lengths[i]-33);
		} else {
			facdata.asm_data->FACHSW = 1L << (bit_lengths[i]-65);
			facdata.asm_data->FACMSW = 0;
		}
		facdata.endpt = (facdata.asm_data->FACHSW * 4294967296.0 + facdata.asm_data->FACMSW) * 4294967296.0 * 2.0;
		stop_reason = factorPassSetup (thread_num, 0, &facdata);
		if (stop_reason) {
			last_bench_cpu_num = NUM_CPUS;
			return (stop_reason);
		}

/* Output start message for this bit length */

		sprintf (buf, "Timing trial factoring of M35000011 with %d bit length factors.  ", bit_lengths[i]);
		OutputStr (thread_num, buf);

/* Do one "iteration" untimed, to prime the caches. */

		res = factorChunk (&facdata);
		(void) factorChunksProcessed (&facdata);

/* Time 10 iterations. Take best time. */

		for (j = 0; j < 10; j++) {
			stop_reason = stopCheck (thread_num);
			if (stop_reason) {
				OutputStrNoTimeStamp (thread_num, "\n");
				OutputStr (thread_num, "Execution halted.\n");
				factorDone (&facdata);
				last_bench_cpu_num = NUM_CPUS;
				return (stop_reason);
			}
			clear_timers (timers, sizeof (timers) / sizeof (timers[0]));
			start_timer (timers, 0);
			res = factorChunk (&facdata);
			end_timer (timers, 0);
			if (j == 0 || timers[0] < best_time) best_time = timers[0] / factorChunksProcessed (&facdata);
		}
		factorDone (&facdata);

/* Print the best time for this bit length.  Take into account that */
/* X86_64 factorChunk code does a different amount of work. */
/* Historically, this benchmark reports timings for processing 16KB of sieve. */

		best_time = best_time / (FACTOR_CHUNK_SIZE / 16.0);
		timers[0] = best_time;
		strcpy (buf, "Best time: ");
		print_timer (timers, 0, buf, TIMER_NL | TIMER_MS);
		OutputStrNoTimeStamp (thread_num, buf);
		sprintf (buf, "Best time for %d bit trial factors: ", bit_lengths[i]);
		print_timer (timers, 0, buf, TIMER_NL | TIMER_MS);
		writeResults (buf);
	}

/* End the threads that are looping and return */

	last_bench_cpu_num = NUM_CPUS;
	return (0);
}

/* Globals and structures used in primeBenchMultipleWorkers */

int	num_bench_workers = 0;
int	num_bench_workers_initialized = 0;
int	bench_worker_finished = 0;
int	bench_workers_time = 0;			/* Time (in seconds) to bench an FFT */
gwmutex	bench_workers_mutex;
gwevent	bench_workers_sync;

struct prime_bench_arg {
	int	main_thread_num;
	unsigned long fftlen;
	int	plus1;
	int	cpu_num;
	int	threads;
	int	hyperthreads;
	int	impl;
	int	iterations;
	int	error_check;
	double	total_time;
};

/* Time a few iterations on one worker. */

void primeBenchOneWorker (void *arg)
{
	struct PriorityInfo sp_info;
	llhandle lldata;
	int	stop_reason;
	double	timers[2];
	struct prime_bench_arg *info;

/* Type cast arg, init return info */

	info = (struct prime_bench_arg *) arg;
	info->iterations = 0;
	info->total_time = 0.0;

/* Set the affinity so that worker runs on the specified CPU core */

	sp_info.type = SET_PRIORITY_BENCHMARKING;
	sp_info.worker_num = info->main_thread_num;
	sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosityBench", 0);
	sp_info.bench_base_cpu_num = info->cpu_num;
	sp_info.bench_hyperthreads = info->hyperthreads;
	sp_info.aux_thread_num = 0;
	SetPriority (&sp_info);

/* Initialize this FFT length */

	gwinit (&lldata.gwdata);
	gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
	gwset_num_threads (&lldata.gwdata, info->threads);
	gwset_thread_callback (&lldata.gwdata, SetAuxThreadPriority);
	gwset_thread_callback_data (&lldata.gwdata, &sp_info);
	lldata.gwdata.bench_pick_nth_fft = info->impl;
	stop_reason = lucasSetup (info->main_thread_num, info->fftlen * 17 + 1, info->fftlen + info->plus1, &lldata);
	if (stop_reason) {
		gwevent_signal (&bench_workers_sync);
		return;
	}

/* Fill data space with random values. */

	generateRandomData (&lldata);

/* Pause until all worker threads are initialized */

	gwmutex_lock (&bench_workers_mutex);
	num_bench_workers_initialized++;
	if (num_bench_workers_initialized == num_bench_workers) gwevent_signal (&bench_workers_sync);
	gwmutex_unlock (&bench_workers_mutex);
	gwevent_wait (&bench_workers_sync, 0);

/* Do one squaring untimed, to prime the caches and start the POSTFFT optimization going. */

	gwsetnormroutine (&lldata.gwdata, 0, info->error_check, 0);
	gwstartnextfft (&lldata.gwdata, TRUE);
	gwsquare (&lldata.gwdata, lldata.lldata);

/* Compute numbers in the lucas series.  Keep track of the number of iterations and total time. */

	for ( ; ; ) {
		stop_reason = stopCheck (info->main_thread_num);
		if (stop_reason) break;
		clear_timers (timers, sizeof (timers) / sizeof (timers[0]));
		start_timer (timers, 0);
		gwsquare (&lldata.gwdata, lldata.lldata);
		end_timer (timers, 0);
		// If any of the bench workers have finished, then do not imclude this timing
		if (bench_worker_finished) break;
		// Add in this timing.  If time exceeds our limit, end this worker
		info->iterations++;
		info->total_time += timer_value (timers, 0);
		if (info->total_time > bench_workers_time) {
			bench_worker_finished = TRUE;
			break;
		}
	}
	lucasDone (&lldata);
}

/* Time an FFT for a few seconds on multiple workers.  This let's us */
/* benchmark the effects of memory bandwidth limitations. */

int primeBenchMultipleWorkersInternal (
	int	thread_num,
	struct primenetBenchmarkData *pkt,
	unsigned long min_FFT_length,
	unsigned long max_FFT_length,
	int	only_time_5678,
	int	time_all_complex,
	int	all_bench,
	const char *bench_cores,
	int	bench_hyperthreading,
	const char *bench_workers,
	int	bench_arch,
	int	bench_oddballs,
	int	bench_error_check,
	int	min_cores,
	int	max_cores,
	int	incr_cores,
	int	min_workers,
	int	max_workers,
	int	incr_workers)
{
	llhandle lldata;
	char	buf[4096];
	int	workers, cpus, hypercpus, impl;
	int	plus1, is_a_5678;
	int	i, stop_reason;
	unsigned long fftlen;
	double	throughput;
	gwthread thread_id[MAX_NUM_WORKER_THREADS];
	struct prime_bench_arg info[MAX_NUM_WORKER_THREADS];

/* Init the worker synchronization primitives */

	gwmutex_init (&bench_workers_mutex);
	gwevent_init (&bench_workers_sync);

/* Loop over a variety of FFT lengths */

	for (plus1 = 0; plus1 <= 1; plus1++) {
	  if (plus1 == 0 && time_all_complex == 2) continue;
	  if (plus1 == 1 && time_all_complex == 0) continue;
	  for (fftlen = min_FFT_length * 1024; fftlen <= max_FFT_length * 1024; fftlen += 10) {

/* Initialize this FFT length */

	    gwinit (&lldata.gwdata);
	    gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
	    if (all_bench) lldata.gwdata.bench_pick_nth_fft = 1;
	    stop_reason = lucasSetup (thread_num, fftlen * 17 + 1, fftlen + plus1, &lldata);
	    if (stop_reason) {
		    if (all_bench) gwbench_write_data ();	/* Write accumulated benchmark data to gwnum.txt */
		    gwmutex_destroy (&bench_workers_mutex);
		    gwevent_destroy (&bench_workers_sync);
		    return (stop_reason);
	    }

/* Make sure the FFT length is within the range we are benchmarking */

	    fftlen = gwfftlen (&lldata.gwdata);
	    if (fftlen > max_FFT_length * 1024) {
		    lucasDone (&lldata);
		    break;
	    }

/* Only bench FFT lengths that are a multiple of 1K */

	    if (fftlen & 0x3FF) {
		    lucasDone (&lldata);
		    continue;
	    }

/* If requested, only bench PFAs of 5,6,7,8 */

	    for (i = fftlen; i >= 9 && (i & 1) == 0; i >>= 1);
	    is_a_5678 = (i <= 8);
	    if (only_time_5678 && !is_a_5678) {
		    lucasDone (&lldata);
		    continue;
	    }

/* If requested, only benchmark one architecture */

	    if (bench_arch && bench_arch != lldata.gwdata.ARCH) {
		    lucasDone (&lldata);
		    continue;
	    }

/* If timing all implementations of an FFT, loop through all possible implementations */

	    for (impl = 1; ; impl++) {
		if (impl > 1) {
			if (!all_bench) break;
			lucasDone (&lldata);
			gwinit (&lldata.gwdata);
			gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
			lldata.gwdata.bench_pick_nth_fft = impl;
			stop_reason = lucasSetup (thread_num, fftlen * 17 + 1, fftlen + plus1, &lldata);
			if (stop_reason) break;	// Assume stop_reason set because there are no more implementations for this FFT
		}

/* Loop over all possible multithread possibilities */

		for (cpus = min_cores; cpus <= max_cores; cpus += incr_cores) {
		    if (! is_number_in_list (cpus, bench_cores)) continue;
		    for (hypercpus = 1; hypercpus <= (int) CPU_HYPERTHREADS; hypercpus++) {
			if (hypercpus > 1 && !bench_hyperthreading) break;
			/* If the OS cannot set affinity, then we can only bench hyperthreading on all CPUs */
			if (!OS_CAN_SET_AFFINITY && hypercpus > 1 && cpus != NUM_CPUS) continue;
			for (workers = min_workers; workers <= cpus && workers <= max_workers; workers += incr_workers) {
			    int	cores_per_node, nodes_left, workers_left, worker_num, cores_left, core_num;

			    if (! is_number_in_list (workers, bench_workers)) continue;
			    if (cpus % workers != 0 && !bench_oddballs) continue;
			    /* Only SSE2 code supports multi-threaded FFTs */
			    if ((cpus > workers || hypercpus > 1) && ! (CPU_FLAGS & CPU_SSE2)) continue;

/* Output start message for this benchmark */

			    sprintf (buf, "Timing %luK%s FFT, %d core%s%s, %d worker%s.  ",
				     fftlen / 1024, plus1 ? " all-complex" : "",
				     cpus, cpus > 1 ? "s" : "",
				     hypercpus > 1 ? " hyperthreaded" : "",
				     workers, workers > 1 ? "s" : "");
			    OutputStr (thread_num, buf);

/* Start the workers */

			    num_bench_workers = workers;
			    num_bench_workers_initialized = 0;
			    bench_worker_finished = FALSE;
			    cores_per_node = NUM_CPUS / NUM_THREADING_NODES;
			    nodes_left = NUM_THREADING_NODES;
			    workers_left = workers;
			    cores_left = cpus;
			    worker_num = 0;
			    gwevent_reset (&bench_workers_sync);
			    while (workers_left) {
				int	nodes_to_use, workers_this_node, cores_this_node;
				core_num = (NUM_THREADING_NODES - nodes_left) * cores_per_node;
				nodes_to_use = divide_rounding_up (nodes_left, workers_left);		// ceil (nodes_left / workers_left)
				workers_this_node = divide_rounding_up (workers_left, nodes_left);	// ceil (workers_left / nodes_left)
				if (workers_this_node == 1)
					cores_this_node = divide_rounding_up (cores_left, workers_left);// ceil (cores_left / workers_left)
				else
					cores_this_node = divide_rounding_up (cores_left, nodes_left);	// ceil (cores_left / nodes_left)
				for ( ; workers_this_node; workers_this_node--) {
				    int	cores_to_use = cores_this_node / workers_this_node;
				    info[worker_num].main_thread_num = thread_num;
				    info[worker_num].fftlen = fftlen;
				    info[worker_num].plus1 = plus1;
				    info[worker_num].impl = (all_bench ? impl : 0);
				    info[worker_num].cpu_num = core_num;
				    info[worker_num].threads = cores_to_use * hypercpus;
				    info[worker_num].hyperthreads = hypercpus;
				    info[worker_num].error_check = bench_error_check;
				    gwthread_create_waitable (&thread_id[worker_num], &primeBenchOneWorker, (void *) &info[worker_num]);
				    core_num += cores_to_use;
				    cores_this_node -= cores_to_use;
				    worker_num++;
				    workers_left--; 
				}
				nodes_left -= nodes_to_use;
				cores_left -= nodes_to_use * cores_per_node;
			    }

/* Wait for all the workers to finish */

			    for (i = 0; i < workers; i++)
				gwthread_wait_for_exit (&thread_id[i]);
			    stop_reason = stopCheck (thread_num);
			    if (stop_reason) {
				    if (all_bench) gwbench_write_data ();	/* Write accumulated benchmark data to gwnum.txt */
				    gwmutex_destroy (&bench_workers_mutex);
				    gwevent_destroy (&bench_workers_sync);
				    return (stop_reason);
			    }

/* Print the total throughput and average times for this FFT length */

			    strcpy (buf, "Average times: ");
			    throughput = 0.0;
			    for (i = 0; i < workers; i++) {
				if (i) strcat (buf, ", ");
				if (info[i].iterations) {
					sprintf (buf+strlen(buf), "%5.2f", info[i].total_time / info[i].iterations * 1000.0);
					throughput = throughput + info[i].iterations / info[i].total_time;
				} else
					strcat (buf, "INF");
			    }
			    sprintf (buf+strlen(buf), " ms.  Total throughput: %5.2f iter/sec.\n", throughput);
			    OutputStrNoTimeStamp (thread_num, buf);

/* Output to the results file the total throughput and average times for this FFT length */

			    if (all_bench) {
				sprintf (buf,
					 "FFTlen=%luK%s, Type=%d, Arch=%d, Pass1=%lu, Pass2=%lu, clm=%lu",
					 fftlen / 1024, plus1 ? " all-complex" : "",
					 lldata.gwdata.FFT_TYPE, lldata.gwdata.ARCH,
					 fftlen / (lldata.gwdata.PASS2_SIZE ? lldata.gwdata.PASS2_SIZE : 1),
					 lldata.gwdata.PASS2_SIZE,
					 lldata.gwdata.PASS1_CACHE_LINES / ((CPU_FLAGS & CPU_AVX) ? 4 : 2));
			    } else {
				sprintf (buf, "Timings for %luK%s FFT length",
					 fftlen / 1024, plus1 ? " all-complex" : "");
			    }
			    if (hypercpus <= 2)
				sprintf (buf+strlen(buf), " (%d core%s%s, %d worker%s): ",
					 cpus, cpus > 1 ? "s" : "",
					 hypercpus > 1 ? " hyperthreaded" : "",
					 workers, workers > 1 ? "s" : "");
			    else
				sprintf (buf+strlen(buf), " (%d core%s, %d threads, %d worker%s): ",
					 cpus, cpus > 1 ? "s" : "",
					 hypercpus * cpus,
					 workers, workers > 1 ? "s" : "");

			    throughput = 0.0;
			    for (i = 0; i < workers; i++) {
				if (i) strcat (buf, ", ");
				if (info[i].iterations) {
					sprintf (buf+strlen(buf), "%5.2f", info[i].total_time / info[i].iterations * 1000.0);
					throughput = throughput + info[i].iterations / info[i].total_time;
				} else
					strcat (buf, "INF");
			    }
			    sprintf (buf+strlen(buf), " ms.  Throughput: %5.2f iter/sec.\n", throughput);
			    writeResults (buf);

/* Write the benchmark data to gwnum's SQL database so that gwnum can select the FFT implementation with the best throughput */

			    if (all_bench) {
				struct gwbench_add_struct bench_data;
				bench_data.version = GWBENCH_ADD_VERSION;
				bench_data.throughput = throughput;
				bench_data.bench_length = bench_workers_time;
				bench_data.num_cores = cpus;
				bench_data.num_workers = workers;
				bench_data.num_hyperthreads = hypercpus;
				bench_data.error_checking = bench_error_check;
				gwbench_add_data (&lldata.gwdata, &bench_data);
			    }

/* Accumulate best throughput numbers to send to the server.  We send the non-hyperthreaded */
/* all-cores timings for FFT lengths from 1M on up.  These timings should prove more useful in */
/* comparing which CPUs are the most powerful. */

			    if (!all_bench && is_a_5678 && !plus1 && cpus == NUM_CPUS && hypercpus == 1 && fftlen / 1024 >= 1024)
				add_bench_data_to_pkt (pkt, "TP%luK", fftlen, throughput, FALSE);

/* Benchmark next FFT */

			} // End workers loop
		    } // End hypercpus loop
		} // End cpus loop
		lucasDone (&lldata);
	    }  // End impl loop
	  }  // End fftlen loop
	}  // End plus1 loop
	OutputStr (thread_num, "\n");

/* Write the benchmark data to gwnum.txt so that gwnum can select the FFT implementations with the best throughput */

	if (all_bench) gwbench_write_data ();

/* Output completion message, cleanup and return */

	OutputStr (thread_num, "Throughput benchmark complete.\n");
	gwmutex_destroy (&bench_workers_mutex);
	gwevent_destroy (&bench_workers_sync);
	return (0);
}

/* Time an FFT for a few seconds on multiple workers.  This let's us */
/* benchmark the effects of memory bandwidth limitations. */

int primeBenchMultipleWorkers (
	int	thread_num)
{
	char	bench_cores[512], bench_workers[512];
	int	min_cores, max_cores, incr_cores, min_workers, max_workers, incr_workers;
	int	all_bench, stop_reason;
	struct primenetBenchmarkData pkt;

/* Init */

	memset (&pkt, 0, sizeof (pkt));
	strcpy (pkt.computer_guid, COMPUTER_GUID);

/* Output some initial informative text */

	OutputStr (thread_num, "Benchmarking multiple workers to measure the impact of memory bandwidth\n");

/* Get the amount of time to bench each FFT */

	bench_workers_time = IniGetInt (INI_FILE, "BenchTime", 10);

/* Get which cores/workers/implementations to time */

	IniGetString (INI_FILE, "BenchCores", bench_cores, sizeof(bench_cores), NULL); /* CPU cores to benchmark (comma separated list) */
	IniGetString (INI_FILE, "BenchWorkers", bench_workers, sizeof(bench_workers), NULL); /* Workers to benchmark (comma separated list) */
	all_bench = IniGetInt (INI_FILE, "AllBench", 0);		/* Benchmark all implementations of each FFT length */

/* For CPUs with tons of cores, we support INI settings to limit number of cores and workers to test */
/* This feature is pretty much obsolete now that dialog-box accepts comma-separated lists. */

	min_cores = IniGetInt (INI_FILE, "BenchMinCores", 1);
	if (min_cores < 1) min_cores = 1;
	if (min_cores > (int) NUM_CPUS) min_cores = NUM_CPUS;

	max_cores = IniGetInt (INI_FILE, "BenchMaxCores", NUM_CPUS);
	if (max_cores < 1) max_cores = 1;
	if (max_cores > (int) NUM_CPUS) max_cores = NUM_CPUS;

	incr_cores = IniGetInt (INI_FILE, "BenchCoresIncrement", 1);
	if (incr_cores < 1) incr_cores = 1;

	min_workers = IniGetInt (INI_FILE, "BenchMinWorkers", 1);
	if (min_workers < 1) min_workers = 1;
	if (min_workers > (int) NUM_CPUS) min_workers = NUM_CPUS;

	max_workers = IniGetInt (INI_FILE, "BenchMaxWorkers", NUM_CPUS);
	if (max_workers < 1) max_workers = 1;
	if (max_workers > (int) NUM_CPUS) max_workers = NUM_CPUS;

	incr_workers = IniGetInt (INI_FILE, "BenchWorkersIncrement", 1);
	if (incr_workers < 1) incr_workers = 1;

/* Do the throughput benchmark */

	stop_reason = primeBenchMultipleWorkersInternal (
		thread_num,
		&pkt,
		IniGetInt (INI_FILE, "MinBenchFFT", 1024),
		IniGetInt (INI_FILE, "MaxBenchFFT", 8192),
		IniGetInt (INI_FILE, "OnlyBench5678", 0),			/* Limit FFTs benched to mimic previous prime95s */
		IniGetInt (INI_FILE, "BenchAllComplex", 0),
		all_bench,
		bench_cores,
		IniGetInt (INI_FILE, "BenchHyperthreads", 1),			/* Benchmark hyperthreading */
		bench_workers,
		IniGetInt (INI_FILE, "BenchArch", 0),				/* CPU architecture to benchmark */
		IniGetInt (INI_FILE, "BenchOddWorkers", 1),			/* Benchmark oddball worker/core combinations */
		IniGetInt (INI_FILE, "BenchErrorCheck", 0),			/* Benchmark round-off checking */
		min_cores,
		max_cores,
		incr_cores,
		min_workers,
		max_workers,
		incr_workers);

/* Write the benchmark data to gwnum.txt so that gwnum can select the FFT implementations with the best throughput */

	if (all_bench) gwbench_write_data ();

/* If benchmark did not complete, return */

	if (stop_reason) return (stop_reason);

/* Send the benchmark data to the server. */

//bug - send bench data to server. (checkbox to allow sending data to server?)
//only do this if guid is registered? Or should we auto-register computer
//under ANONYMOUS userid for stress-testers.

	if (pkt.num_data_points)
		spoolMessage (PRIMENET_BENCHMARK_DATA, &pkt);

/* Output completion message, cleanup and return */

	OutputStr (thread_num, "Throughput benchmark complete.\n");
	return (0);
}

/* Perform automatic benchmarks.  Scan worktodo and examine amount of benchmarking data we already have to */
/* decide if more throughput benchmarks are needed for selecting optimal FFT implementations. */

void autoBench (void)
{
	char	bench_cores[10], bench_workers[10];
	int	num_cores, num_workers, autobench_num_benchmarks, tnum, i, num_ffts_to_bench;
	double	autobench_days_of_work;
	struct {
		unsigned long min_fftlen;
		unsigned long max_fftlen;
		int	all_complex;
	} ffts_to_bench[200];
	struct primenetBenchmarkData pkt;

/* If workers are not active, do not benchmark now */

	if (!WORKER_THREADS_ACTIVE || WORKER_THREADS_STOPPING) return;

/* If we're not supposed to run while on battery and we are on battery power now, then skip this benchmark */

	if (!RUN_ON_BATTERY && OnBattery ()) return;

/* If any worker is paused due to PauseWhileRunning, then skip auto-bench */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++) if (STOP_FOR_PAUSE[i] != NULL) return;

/* If there are any threads with high variable memory usage, then skip auto-bench. */
/* We do this because restarting stage 2 can take a long time when the worker is using lots of memory. */

	for (i = 0; i < (int) NUM_WORKER_THREADS; i++)
		if ((MEM_FLAGS[i] & (MEM_VARIABLE_USAGE | MEM_WILL_BE_VARIABLE_USAGE)) && MEM_IN_USE[i] >= 250) return;

// BUG/FEATURE -- purge bench DB of old or anomalous results so that we can replace them with new, hopefully accurate, benchmarks?

/* Calculate number of cores/workers used in FFT selection process */

	if (BENCH_NUM_CORES)
		num_cores = BENCH_NUM_CORES;		/* Use gwnum.txt override or default to all cores in use */
	else {
		num_cores = 0;
		for (i = 0; i < (int) NUM_WORKER_THREADS; i++) num_cores += CORES_PER_TEST[i];
		if (num_cores > (int) NUM_CPUS) num_cores = NUM_CPUS;
	}
	num_workers = BENCH_NUM_WORKERS ? BENCH_NUM_WORKERS : NUM_WORKER_THREADS; /* Use gwnum.txt override or default to all workers */

/* Get some ini file overrides for autobenching criteria. */

	autobench_days_of_work = (double) IniGetInt (INI_FILE, "AutoBenchDaysOfWork", 7);
	autobench_num_benchmarks = IniGetInt (INI_FILE, "AutoBenchNumBenchmarks", 10);

/* Look at worktodo.txt for FFT sizes we are working on now or will work on soon.  See if any need more benchmark data. */
/* Loop over all worker threads */

	num_ffts_to_bench = 0;
	for (tnum = 0; tnum < (int) NUM_WORKER_THREADS; tnum++) {
	    struct work_unit *w;
	    double	est;

/* Loop over all work units */

	    w = NULL;
	    est = 0.0;
	    for ( ; ; ) {
		int	all_complex, num_benchmarks;
		unsigned long min_fftlen, max_fftlen;    

/* Read the next line of the work file */

		w = getNextWorkToDoLine (tnum, w, SHORT_TERM_USE);
		if (w == NULL) break;
		if (w->work_type == WORK_NONE) continue;

/* Ignore any worktodo entries that will not begin within the next 7 days.  No particular reason for this. */
/* This reduces the likelihood of an excessively long autobench.  The worktodo entry might be deleted before */
/* before it gets in the 7-day window.  On the downside, we'll only get 7 autobenches in which may not be */
/* completely accurate.  Also, work_estimates are notoriously inaccurate. */

		if (est > autobench_days_of_work * 86400.0) continue;
		est += work_estimate (tnum, w);

/* Trial factoring is the only work type that does not use FFTs */

		if (w->work_type == WORK_FACTOR) continue;

/* If this is an LL test determine the FFT size that will actually be used */

		if (w->work_type == WORK_TEST || w->work_type == WORK_DBLCHK || w->work_type == WORK_ADVANCEDTEST)
			pick_fft_size (MAIN_THREAD_NUM, w);

/* Ask gwnum how many relevant benchmarks are in its database */
/* If we have enough benchmarks, skip this worktodo entry */

		gwbench_get_num_benchmarks (w->k, w->b, w->n, w->c, w->minimum_fftlen, num_cores, num_workers, HYPERTHREAD_LL, ERRCHK,
					    &min_fftlen, &max_fftlen, &all_complex, &num_benchmarks);
		if (num_benchmarks >= autobench_num_benchmarks) continue;

/* For this release, we do not run automatic benchmarks for FFT lengths below 8K */

		if (max_fftlen < 8192) continue;
		if (min_fftlen < 8192) min_fftlen = 8192;

/* Add the returned FFT lengths to our list of FFTs to benchmark */

		for (i = 0; ; i++) {
			if (i == num_ffts_to_bench) {
				ffts_to_bench[num_ffts_to_bench].min_fftlen = min_fftlen;
				ffts_to_bench[num_ffts_to_bench].max_fftlen = max_fftlen;
				ffts_to_bench[num_ffts_to_bench].all_complex = all_complex;
				num_ffts_to_bench++;
				break;
			}
			if (ffts_to_bench[i].all_complex != all_complex) continue;
			if (min_fftlen >= ffts_to_bench[i].min_fftlen && min_fftlen <= ffts_to_bench[i].max_fftlen) {
				if (max_fftlen > ffts_to_bench[i].max_fftlen) ffts_to_bench[i].max_fftlen = max_fftlen;
				break;
			}
			if (max_fftlen >= ffts_to_bench[i].min_fftlen && max_fftlen <= ffts_to_bench[i].max_fftlen) {
				if (min_fftlen < ffts_to_bench[i].min_fftlen) ffts_to_bench[i].min_fftlen = min_fftlen;
				break;
			}
		}
	    }
	}

/* If we did not find any FFT lengths that need more benchmark data, then we are done! */

	if (num_ffts_to_bench == 0) return;

/* Init pkt to send to server */

	memset (&pkt, 0, sizeof (pkt));
	strcpy (pkt.computer_guid, COMPUTER_GUID);

/* Output some initial informative text */

	OutputStr (MAIN_THREAD_NUM, "Benchmarking multiple workers to tune FFT selection.\n");

/* Init various benchmarking parameters */

	sprintf (bench_cores, "%d", num_cores);
	sprintf (bench_workers, "%d", num_workers);
	bench_workers_time = IniGetInt (INI_FILE, "AutoBenchTime", 12);		/* Amount of time to bench each FFT */

/* Stop workers for autobenchmarks */

	gwevent_reset (&AUTOBENCH_EVENT);
	STOP_FOR_AUTOBENCH = TRUE;

/* Wait a few seconds for workers to stop */
/* BUG/FEATURE -- it would be nice to know for certain that all workers have responded to stop_for_autobench */

	Sleep (3000);

/* Loop through list of FFTs needing more bench data.  Do the throughput benchmarks. */

// BUG/FEATURE  sort by FFTlen? / sort by date needed? and limit amount of time?? overkill?

	for (i = 0; i < num_ffts_to_bench; i++) {
		int	stop_reason;

//BUG		option for redoing just top X% of FFT impls?  why redo the obvious losers?  uneven number of benchmarks per impl
// might mean gwbench_get_num_benchmarks SQL stmt needs tweaking

		stop_reason = primeBenchMultipleWorkersInternal (
			MAIN_THREAD_NUM,					/* Output messages to main window */
			&pkt,
			ffts_to_bench[i].min_fftlen / 1024,			/* Minimum FFT length (in K) to bench */
			ffts_to_bench[i].max_fftlen / 1024,			/* Maximum FFT length (in K) to bench */
			FALSE,							/* Do not limit FFT sizes benchmarked */
			ffts_to_bench[i].all_complex,
			TRUE,							/* Benchmark all FFT implementations */
			bench_cores,
			HYPERTHREAD_LL,						/* Benchmark hyperthreading if LL testing uses hyperthreads */
			bench_workers,
			0,							/* Do not limit CPU architectures benchmarked */
			0,							/* Oddball worker/core combinations option does not apply */
			ERRCHK,							/* Benchmark round-off checking */
			num_cores,						/* Min cores */
			num_cores,						/* Max cores */
			1,							/* Core increment */
			num_workers,						/* Min workers */
			num_workers,						/* Max workers */
			1);							/* Worker increment */

/* If benchmark was stopped, run no more benchmarks */

		if (stop_reason) break;
	}

/* Write the benchmark data to gwnum.txt so that gwnum can select the FFT implementations with the best throughput */

	gwbench_write_data ();

/* Send the benchmark data to the server??? */
//bug - do we want to send autobench bench data to server???
//
//	if (pkt.num_data_points)
//		spoolMessage (PRIMENET_BENCHMARK_DATA, &pkt);

/* Since restarting workers will run a Jacobi test on the latest save files, we restart the Jacobi timer */
	
	memset (JACOBI_ERROR_CHECK, 0, sizeof (JACOBI_ERROR_CHECK));
	start_Jacobi_timer ();

/* Restart workers */

	STOP_FOR_AUTOBENCH = FALSE;
	gwevent_signal (&AUTOBENCH_EVENT);
}

/* Perform a benchmark.  Several are supported:  FFT throughput, FFT timings, trial factoring */

int primeBench (
	int	thread_num,
	int	bench_type)
{
	struct PriorityInfo sp_info;
	llhandle lldata;
	unsigned long i, impl, j, iterations;
	double	best_time, total_time;
	char	buf[512];
	char	bench_cores[512];
	int	min_cores, max_cores, incr_cores, cpu, hypercpu;
	int	all_bench, only_time_5678, time_all_complex, plus1, stop_reason;
	int	is_a_5678, bench_hyperthreading, bench_arch;
	unsigned long fftlen, min_FFT_length, max_FFT_length;
	double	timers[2];
	struct primenetBenchmarkData pkt;

/* Output startup message */

	title (thread_num, "Benchmarking");
	OutputStr (thread_num, BENCH1);
	OutputBoth (thread_num, BENCH2);

/* Output to the results file a full CPU description */

	getCpuDescription (buf, 1);
	writeResults (buf);
	if (IniGetInt (INI_FILE, "BenchOutputTopology", 1)) {
		writeResults ("Machine topology as determined by hwloc library:\n");
		topology_print_children (hwloc_get_root_obj (hwloc_topology), 0);
	}

#ifdef X86_64
	sprintf (buf, "Prime95 64-bit version %s, RdtscTiming=%d\n", VERSION, RDTSC_TIMING);
#else
	sprintf (buf, "Prime95 32-bit version %s, RdtscTiming=%d\n", VERSION, RDTSC_TIMING);
#endif
	writeResults (buf);

/* Set the process/thread priority AFTER getting the CPU description. */
/* This is required so that any threads spawned by getCpuSpeed will not */
/* be confined to just one CPU core. */

	sp_info.type = SET_PRIORITY_BENCHMARKING;
	sp_info.worker_num = thread_num;
	sp_info.verbose_flag = IniGetInt (INI_FILE, "AffinityVerbosityBench", 0);
	sp_info.aux_thread_num = 0;
	sp_info.bench_base_cpu_num = 0;
	sp_info.bench_hyperthreads = 1;
	SetPriority (&sp_info);

/* Perform the requested type of benchmark */

/* Throughput benchmark running multiple workers.  This will measure the effect of memory bandwidth */
/* on LL testing.  This is the most useful benchmark for the typical GIMPS user. */

	if (bench_type == 0) {
		return (primeBenchMultipleWorkers (thread_num));
	}

/* Trial factoring benchmark. */

	if (bench_type == 2) {
		return (factorBench (thread_num));
	}

/* Fall through to the classic FFT timings benchmark. */

/* Init */

	memset (&pkt, 0, sizeof (pkt));
	strcpy (pkt.computer_guid, COMPUTER_GUID);

/* Decide which FFT lengths to time */

	min_FFT_length = IniGetInt (INI_FILE, "MinBenchFFT", 1024);
	max_FFT_length = IniGetInt (INI_FILE, "MaxBenchFFT", 8192);
	only_time_5678 = IniGetInt (INI_FILE, "OnlyBench5678", 1);
	time_all_complex = IniGetInt (INI_FILE, "BenchAllComplex", 0);
	all_bench = 0; //IniGetInt (INI_FILE, "AllBench", 0);			/* Benchmark all implementations of each FFT length */
	bench_arch = IniGetInt (INI_FILE, "BenchArch", 0);			/* CPU architecture to benchmark */
	IniGetString (INI_FILE, "BenchCores", bench_cores, sizeof(bench_cores), NULL); /* Cpu cores to benchmark (comma separated list) */
	bench_hyperthreading = IniGetInt (INI_FILE, "BenchHyperthreads", 1);	/* Benchmark hyperthreading */

/* Keep CPU cores busy.  This should prevent "turbo boost" from kicking in. */
/* We do this to hopefully produce more consistent benchmarks.  We don't want to report */
/* a CPU speed of 1.87 GHz and then produce a benchmark running at a boosted 3.2 GHz */
/* (this happens on a Core i7 Q840M processor). */

	if (IniGetInt (INI_FILE, "BenchDummyWorkers", 0)) {
		last_bench_cpu_num = 0;			// CPU #0 is benching
		for (i = 1; i < NUM_CPUS; i++) {	// CPU #1 to NUM_CPUS-1 are busy looping
			gwthread thread_id;
			gwthread_create (&thread_id, &bench_busy_loop, (void *) (intptr_t) i);
		}
	}

/* For CPUs with tons of cores, we support INI settings to limit number of cores to test */
/* This feature is pretty much obsolete. */

	min_cores = IniGetInt (INI_FILE, "BenchMinCores", 1);
	if (min_cores < 1) min_cores = 1;
	if (min_cores > (int) NUM_CPUS) min_cores = NUM_CPUS;

	max_cores = IniGetInt (INI_FILE, "BenchMaxCores", NUM_CPUS);
	if (max_cores < 1) max_cores = 1;
	if (max_cores > (int) NUM_CPUS) max_cores = NUM_CPUS;

	incr_cores = IniGetInt (INI_FILE, "BenchCoresIncrement", 1);
	if (incr_cores < 1) incr_cores = 1;

/* Loop over all possible single-worker multithread possibilities */

	for (cpu = min_cores; cpu <= max_cores; cpu += incr_cores) {
	  if (! is_number_in_list (cpu, bench_cores)) continue;
	  for (hypercpu = 1; hypercpu <= (int) CPU_HYPERTHREADS; hypercpu++) {
	    if (hypercpu > 1 && !bench_hyperthreading) break;
	    /* Only bench hyperthreading on one CPU and all CPUs */
	    if (hypercpu > 1 && cpu != 1 && cpu != NUM_CPUS) continue;
	    /* If the OS cannot set affinity, then we can only bench hyperthreading on all CPUs */
	    if (!OS_CAN_SET_AFFINITY && hypercpu > 1 && cpu != NUM_CPUS) continue;
	    /* Output a message if using multi-threaded FFT */
	    if (cpu > 1 || hypercpu > 1) {
	      if (! (CPU_FLAGS & CPU_SSE2)) continue;  // Only SSE2 code supports multi-threaded FFTs
	      if (CPU_HYPERTHREADS == 1)
	        sprintf (buf, "Timing FFTs using %d cores.\n", cpu);
	      else
	        sprintf (buf, "Timing FFTs using %d threads on %d core%s.\n", cpu * hypercpu, cpu, cpu > 1 ? "s" : "");
	      OutputBoth (thread_num, buf);
	    }

/* Set global that makes sure we are running the correct number of busy loops */

	    last_bench_cpu_num = cpu - 1;

/* Loop over a variety of FFT lengths */

	    for (plus1 = 0; plus1 <= 1; plus1++) {
	      if (plus1 == 0 && time_all_complex == 2) continue;
	      if (plus1 == 1 && time_all_complex == 0) continue;
	      for (fftlen = min_FFT_length * 1024; fftlen <= max_FFT_length * 1024; fftlen += 10) {
	        for (impl = 1; ; impl++) {
		  if (impl > 1 && !all_bench) break;

/* Initialize for this FFT length.  Compute the number of iterations to */
/* time.  This is based on the fact that it doesn't take too long for */
/* my 1400 MHz P4 to run 10 iterations of a 1792K FFT. */

		  gwinit (&lldata.gwdata);
		  gwset_sum_inputs_checking (&lldata.gwdata, SUM_INPUTS_ERRCHK);
		  if (IniGetInt (LOCALINI_FILE, "UseLargePages", 0))
			gwset_use_large_pages (&lldata.gwdata);
		  gwset_num_threads (&lldata.gwdata, cpu * hypercpu);
		  sp_info.bench_base_cpu_num = 0;
		  sp_info.bench_hyperthreads = hypercpu;
		  gwset_thread_callback (&lldata.gwdata, SetAuxThreadPriority);
		  gwset_thread_callback_data (&lldata.gwdata, &sp_info);
		  if (all_bench) lldata.gwdata.bench_pick_nth_fft = impl;
		  stop_reason = lucasSetup (thread_num, fftlen * 17 + 1, fftlen + plus1, &lldata);
		  if (stop_reason) {
			/* An error during all_bench is expected.  Continue on to next FFT length. */
			/* An error during a norm bench is unexpected.  Set fftlen so that we stop benching. */
			if (!all_bench) fftlen = max_FFT_length * 1024;
			break;
		  }
		  fftlen = gwfftlen (&lldata.gwdata);
		  if (fftlen > max_FFT_length * 1024) {
			lucasDone (&lldata);
			break;
		  }

/* Only bench FFT lengths that are a multiple of 1K */

		  if (fftlen & 0x3FF) {
			lucasDone (&lldata);
			break;
		  }

/* If requested, only bench PFAs of 5,6,7,8 */

		  for (i = fftlen; i >= 9 && (i & 1) == 0; i >>= 1);
		  is_a_5678 = (i <= 8);
		  if (only_time_5678 && !is_a_5678) {
			lucasDone (&lldata);
			break;
		  }

/* If requested, only benchmark one architecture */

		  if (bench_arch && bench_arch != lldata.gwdata.ARCH) {
			lucasDone (&lldata);
			continue;
		  }

/* Output a blank line between different FFT lengths when timing all implementations */

		  if (all_bench && impl == 1) writeResults ("\n");

/* Compute the number of iterations to time.  This is based on the fact that it doesn't */
/* take too long for my 1400 MHz P4 to run 10 iterations of a 1792K FFT. */
/* Updated: minimum number of set to 25 for AVX machines */

		  iterations = (unsigned long) (10 * 1792 * CPU_SPEED / 1400 / (fftlen / 1024));
		  if (iterations < 10) iterations = 10;
		  if (iterations < 25 && (CPU_FLAGS & CPU_AVX)) iterations = 25;
		  if (iterations > 100) iterations = 100;

/* Output start message for this FFT length */

		  sprintf (buf, "Timing %lu iterations of %luK%s FFT length%s.  ",
			 iterations, fftlen / 1024,
			 plus1 ? " all-complex" : "",
			 gw_using_large_pages (&lldata.gwdata) ? " using large pages" : "");
		  OutputStr (thread_num, buf);

/* Fill data space with random values. */

		  generateRandomData (&lldata);

/* Do one squaring untimed, to prime the caches and start the */
/* POSTFFT optimization going. */

		  gwsetnormroutine (&lldata.gwdata, 0, 0, 0);
		  gwstartnextfft (&lldata.gwdata, TRUE);
		  gwsquare (&lldata.gwdata, lldata.lldata);

/* Compute numbers in the lucas series */
/* Note that for reasons unknown, we've seen cases where printing out */
/* the times on each iteration greatly impacts P4 timings. */

		  total_time = 0.0;
		  for (j = 0; j < iterations; j++) {
			stop_reason = stopCheck (thread_num);
			if (stop_reason) {
				OutputStrNoTimeStamp (thread_num, "\n");
				OutputStr (thread_num, "Execution halted.\n");
				lucasDone (&lldata);
				last_bench_cpu_num = NUM_CPUS;
				return (stop_reason);
			}
			clear_timers (timers, sizeof (timers) / sizeof (timers[0]));
			start_timer (timers, 0);
			gwsquare (&lldata.gwdata, lldata.lldata);
			end_timer (timers, 0);
			total_time += timers[0];
			if (j == 0 || timers[0] < best_time) best_time = timers[0];
		  }
		  lucasDone (&lldata);

/* Print the best time for this FFT length */

		  timers[0] = best_time;
		  strcpy (buf, "Best time: ");
		  print_timer (timers, 0, buf, TIMER_MS);
		  timers[0] = total_time / iterations;
		  strcat (buf, ", avg time: ");
		  print_timer (timers, 0, buf, TIMER_NL | TIMER_MS);
		  OutputStrNoTimeStamp (thread_num, buf);
		  if (all_bench) {
			sprintf (buf,
				 "Time FFTlen=%luK%s, Type=%d, Arch=%d, Pass1=%lu, Pass2=%lu, clm=%lu: ",
				 fftlen / 1024, plus1 ? " all-complex" : "",
				 lldata.gwdata.FFT_TYPE, lldata.gwdata.ARCH,
				 fftlen / (lldata.gwdata.PASS2_SIZE ? lldata.gwdata.PASS2_SIZE : 1),
				 lldata.gwdata.PASS2_SIZE,
				 lldata.gwdata.PASS1_CACHE_LINES / ((CPU_FLAGS & CPU_AVX) ? 4 : 2));
			timers[0] = best_time;
			print_timer (timers, 0, buf, TIMER_MS);
			timers[0] = total_time / iterations;
			strcat (buf, ", avg: ");
			print_timer (timers, 0, buf, TIMER_NL | TIMER_MS);
		  } else {
			sprintf (buf, "Best time for %luK%s FFT length: ",
				 fftlen / 1024, plus1 ? " all-complex" : "");
			timers[0] = best_time;
			print_timer (timers, 0, buf, TIMER_MS);
			timers[0] = total_time / iterations;
			strcat (buf, ", avg: ");
			print_timer (timers, 0, buf, TIMER_NL | TIMER_MS);
		  }
		  writeResults (buf);

/* Accumulate best times to send to the server.  These are the "classic" best times -- that is, */
/* best non-hyperthreaded single-core timings for FFT lengths from 1M on up. */

		  if (!all_bench && is_a_5678 && !plus1 && cpu == 1 && hypercpu == 1 && fftlen / 1024 >= 1024)
			add_bench_data_to_pkt (&pkt, "FFT%luK", fftlen, timer_value (timers, 0), TRUE);

/* Accumulate best times to send to the server.  We send the non-hyperthreaded all-cores */
/* timings for FFT lengths from 1M on up.  These timings should prove more useful in */
/* comparing which CPU is are most powerful. */

		  if (!all_bench && is_a_5678 && !plus1 && cpu == NUM_CPUS && hypercpu == 1 && fftlen / 1024 >= 1024)
			add_bench_data_to_pkt (&pkt, "AC%luK", fftlen, timer_value (timers, 0), TRUE);

/* Time next FFT */

	        }  // End impl implementation loop
	      }  // End fftlen loop
	    }  // End plus1 loop
	  } // End hyper loop
	} // End cpu loop

/* End the threads that are busy looping */

	last_bench_cpu_num = NUM_CPUS;

/* Single worker benchmark complete */

	OutputStr (thread_num, "FFT timings benchmark complete.\n");

/* Send the benchmark data to the server. */

//bug - send bench data to server. (checkbox to allow sending data to server?)
//only do this if guid is registered? Or should we auto-register computer
//under ANONYMOUS userid for stress-testers.

	if (pkt.num_data_points)
		spoolMessage (PRIMENET_BENCHMARK_DATA, &pkt);

	return (0);
}

/****************************/
/* Probable Prime Test code */
/****************************/

/* PRP state to be written to / read from PRP save files */

struct prp_state {
	gwnum	x;			/* The current value in our left-to-right exponentiation */
	unsigned long counter;		/* Current "iteration" counter */
	unsigned long units_bit;	/* For shifting FFT data -- allows more robust double-checking */
	unsigned long error_count;	/* Count of any errors that have occurred during PRP test */
	unsigned int prp_base;		/* Fermat PRP base, default is 3 */
	int	two_power_opt;		/* TRUE is power of two optimizations are enabled (N adjusted to create long run of squarings) */
	int	residue_type;		/* Type of residue to generate (5 different residue types are supported) */
	int	error_check_type;	/* 0=none, 1=Gerbicz, 2=double-checking */
	int	state;			/* State variable see definitions below */
	gwnum	alt_x;			/* When doing PRP_ERRCHK_DBLCHK, this is the alternate x value */
					/* When doing PRP_ERRCHK_GERBICZ, this is the comparison checksum value being calculated */
	unsigned long alt_units_bit;	/* When doing PRP_ERRCHK_DBLCHK, this is the alternate shift count */
	gwnum	u0;			/* Saved first value of the Gerbicz checksum function */
	gwnum	d;			/* Last computed value of the Gerbicz checksum function */
	unsigned long L;		/* Iterations between multiplies in computing Gerbicz checksum */
	unsigned long start_counter;	/* Counter at start of current Gerbicz or double-check block */
	unsigned long next_mul_counter;	/* Counter when next Gerbicz multiply takes place */
	unsigned long end_counter;	/* Counter when current Gerbicz or double-check block ends */
};

#define PRP_ERRCHK_NONE		0	/* No high-reliability error-checking */
#define PRP_ERRCHK_GERBICZ	1	/* Gerbicz high-reliability error-checking -- very low overhead */
#define PRP_ERRCHK_DBLCHK	2	/* Run PRP twice, comparing residues along the way. Highly-reliable, very expensive. */

#define PRP_STATE_NORMAL		0	/* Normal left-to-right exponentiation */
#define PRP_STATE_DCHK_PASS1		10	/* Do squarings for a while, then rollback counter and switch to pass 2 */
#define PRP_STATE_DCHK_PASS2		11	/* Do squarings for a while, compare vals, then switch back to pass 1 */
#define PRP_STATE_GERB_START_BLOCK	22	/* Determine how many iters are in the next Gerbicz block */
#define PRP_STATE_GERB_MID_BLOCK	23	/* Do squarings for L iterations */
#define PRP_STATE_GERB_MID_BLOCK_MULT	24	/* Do checksum multiply after the L-th squaring */
#define PRP_STATE_GERB_END_BLOCK	25	/* After L^2 squarings, do alt squarings to compute 2nd Gerbicz compare value */
#define PRP_STATE_GERB_END_BLOCK_MULT	26	/* After L alt squarings, do one last mul and then compare checksum values */

/* Write intermediate PRP results to a file */
/* The PRP save file format is: */
/*	u32		magic number  (different for ll, p-1, prp, tf, ecm) */
/*	u32		version number */
/*	double		pct complete */
/*	char(11)	stage */
/*	char(1)		pad */
/*	u32		checksum of following data */
/*	u32		error_count */
/*	u32		iteration counter */
/*	u32		prp base (version number >= 2) */
/*	u32		shift count (version number >= 2) */
/*	u32		power-of-two optimization was used (version number >= 2) */
/*	u32		residue type (version number >= 3) */
/*	u32		error-check type (version number >= 3) */
/*	u32		state (version number >= 3) */
/*	u32		alternate shift count (version number >= 3) */
/*	u32		L - iterations between Gerbicz multiplies (version number >= 3) */
/*	u32		error-checking start counter (version number >= 3) */
/*	u32		error-checking next Gerbicz multiply counter (version number >= 3) */
/*	u32		error-checking end counter (version number >= 3) */
/*	gwnum		FFT data for x (u32 len, array u32s) */
/*	gwnum		FFT data for alt_x (u32 len, array u32s) (version number >= 3) */
/*	gwnum		FFT data for u0 (u32 len, array u32s) (version number >= 3) */
/*	gwnum		FFT data for d (u32 len, array u32s) (version number >= 3) */

#define PRP_MAGICNUM		0x87f2a91b
#define PRP_VERSION		3

int writePRPSaveFile (
	gwhandle *gwdata,
	writeSaveFileState *write_save_file_state,
	struct work_unit *w,
	struct prp_state *ps)
{
	int	fd;
	unsigned long sum = 0;

/* Now save to the intermediate file */

	fd = openWriteSaveFile (write_save_file_state);
	if (fd < 0) return (FALSE);

	if (!write_header (fd, PRP_MAGICNUM, PRP_VERSION, w)) goto err;

	if (!write_long (fd, ps->error_count, &sum)) goto err;
	if (!write_long (fd, ps->counter, &sum)) goto err;
	if (!write_long (fd, ps->prp_base, &sum)) goto err;
	if (!write_long (fd, ps->units_bit, &sum)) goto err;
	if (!write_long (fd, ps->two_power_opt, &sum)) goto err;
	if (!write_long (fd, ps->residue_type, &sum)) goto err;
	if (!write_long (fd, ps->error_check_type, &sum)) goto err;
	if (!write_long (fd, ps->state, &sum)) goto err;
	if (!write_long (fd, ps->alt_units_bit, &sum)) goto err;
	if (!write_long (fd, ps->L, &sum)) goto err;
	if (!write_long (fd, ps->start_counter, &sum)) goto err;
	if (!write_long (fd, ps->next_mul_counter, &sum)) goto err;
	if (!write_long (fd, ps->end_counter, &sum)) goto err;
	if (!write_gwnum (fd, gwdata, ps->x, &sum)) goto err;
	if (ps->state != PRP_STATE_NORMAL && ps->state != PRP_STATE_GERB_START_BLOCK && ps->state != PRP_STATE_GERB_MID_BLOCK) {
		if (!write_gwnum (fd, gwdata, ps->alt_x, &sum)) goto err;
	}
	if (ps->state != PRP_STATE_NORMAL && ps->state != PRP_STATE_GERB_START_BLOCK &&
	    ps->state != PRP_STATE_DCHK_PASS1 && ps->state != PRP_STATE_DCHK_PASS2) {
		if (!write_gwnum (fd, gwdata, ps->u0, &sum)) goto err;
		if (!write_gwnum (fd, gwdata, ps->d, &sum)) goto err;
	}

	if (!write_checksum (fd, sum)) goto err;

	closeWriteSaveFile (write_save_file_state, fd);
	return (TRUE);

/* An error occured.  Delete the current file. */

err:	deleteWriteSaveFile (write_save_file_state, fd);
	return (FALSE);
}

/* Read the data portion of an intermediate PRP results file */

int readPRPSaveFile (
	gwhandle *gwdata,		/* Handle to gwnum */
	char	*filename,		/* Save file name */
	struct work_unit *w,		/* Work unit */
	struct prp_state *ps)		/* PRP state structure to read and fill in */
{
	int	fd;
	unsigned long savefile_prp_base, sum, filesum, version;

	fd = _open (filename, _O_BINARY | _O_RDONLY);
	if (fd <= 0) return (FALSE);

	if (!read_magicnum (fd, PRP_MAGICNUM)) goto err;
	if (!read_header (fd, &version, w, &filesum)) goto err;
	if (version == 0 || version > PRP_VERSION) goto err;

	sum = 0;
	if (!read_long (fd, &ps->error_count, &sum)) goto err;
	if (!read_long (fd, &ps->counter, &sum)) goto err;

	if (version <= 1) {
		savefile_prp_base = 3;
		ps->units_bit = 0;
		ps->two_power_opt = FALSE;
	} else {
		unsigned long savefile_two_power_opt;
		if (!read_long (fd, &savefile_prp_base, &sum)) goto err;
		if (!read_long (fd, &ps->units_bit, &sum)) goto err;
		if (!read_long (fd, &savefile_two_power_opt, &sum)) goto err;
		ps->two_power_opt = savefile_two_power_opt;
	}
	if (savefile_prp_base != ps->prp_base) goto err;

	if (version <= 2) {
		ps->state = PRP_STATE_NORMAL;
		ps->error_check_type = PRP_ERRCHK_NONE;
	} else {
		unsigned long savefile_state, savefile_residue_type, savefile_error_check_type;
		// We might be able to handle some mismatched residue types, for now don't since this should never happen
		if (!read_long (fd, &savefile_residue_type, &sum)) goto err;
		if (savefile_residue_type != ps->residue_type) goto err;
		// We could handle some mismatched error check types by looking at the state
		// variable, for now don't since this should never happen
		if (!read_long (fd, &savefile_error_check_type, &sum)) goto err;
		if (savefile_error_check_type != ps->error_check_type) goto err;
		if (!read_long (fd, &savefile_state, &sum)) goto err;
		ps->state = savefile_state;
		if (!read_long (fd, &ps->alt_units_bit, &sum)) goto err;
		if (!read_long (fd, &ps->L, &sum)) goto err;
		if (!read_long (fd, &ps->start_counter, &sum)) goto err;
		if (!read_long (fd, &ps->next_mul_counter, &sum)) goto err;
		if (!read_long (fd, &ps->end_counter, &sum)) goto err;
	}

	if (!read_gwnum (fd, gwdata, ps->x, &sum)) goto err;
	if (ps->state != PRP_STATE_NORMAL && ps->state != PRP_STATE_GERB_START_BLOCK && ps->state != PRP_STATE_GERB_MID_BLOCK) {
		if (!read_gwnum (fd, gwdata, ps->alt_x, &sum)) goto err;
	}
	if (ps->state != PRP_STATE_NORMAL && ps->state != PRP_STATE_GERB_START_BLOCK &&
	    ps->state != PRP_STATE_DCHK_PASS1 && ps->state != PRP_STATE_DCHK_PASS2) {
		if (!read_gwnum (fd, gwdata, ps->u0, &sum)) goto err;
		if (!read_gwnum (fd, gwdata, ps->d, &sum)) goto err;
	}

	if (filesum != sum) goto err;
	_close (fd);
	return (TRUE);
err:	_close (fd);
	return (FALSE);
}

/* Output the good news of a new probable prime to the screen in an infinite loop */

void good_news_prp (void *arg)
{
	char	buf[800];
	int	i = 0;

	title (MAIN_THREAD_NUM, "New Probable Prime!!!");
	sprintf (buf, "New Probable Prime!!!!  %s is a probable prime!\n", (char *) arg);
	while (WORKER_THREADS_ACTIVE && ! WORKER_THREADS_STOPPING) {
		if ((i++ & 127) == 0) OutputStr (MAIN_THREAD_NUM, buf);
		flashWindowAndBeep ();
		Sleep (50);
	}
	free (arg);
}

/* Compare two (possibly shifted) gwnums for equality.  Used in PRP error-checking. */

int areTwoPRPValsEqual (
	gwhandle *gwdata,
	unsigned long p,		/* Mersenne exponent (for shifting) */		
	gwnum	val1,			/* Value #1 */
	unsigned long units_bit1,	/* Shift count #1 */
	gwnum	val2,			/* Value #2 */
	unsigned long units_bit2)	/* Shift count #2 */
{
	giant	tmp1, tmp2;
	int	diff;

	tmp1 = popg (&gwdata->gdata, ((unsigned long) gwdata->bit_length >> 5) + 5);
	gwtogiant (gwdata, val1, tmp1);
	rotateg (tmp1, p, units_bit1, &gwdata->gdata);
	tmp2 = popg (&gwdata->gdata, ((unsigned long) gwdata->bit_length >> 5) + 5);
	gwtogiant (gwdata, val2, tmp2);
	rotateg (tmp2, p, units_bit2, &gwdata->gdata);
	diff = gcompg (tmp1, tmp2);
	pushg (&gwdata->gdata, 2);
	return (!diff);
}

/* Mul giant by a power of the PRP base.  Used in optimizing PRPs for k*2^n+c numbers. */

void basemulg (
	giant	v,			/* Giant to multiply by base^power */
	struct work_unit *w,		/* Number being tested */
	unsigned int prp_base,		/* PRP base */
	int	power)			/* Desired power of the PRP base */
{
	mpz_t	modulus, prp_base_power, tmp;

/* If power is zero, then multiply by base^0 is a no-op */

	if (power == 0) return;

/* Generate the modulus (k*b^n+c), b is known to be 2 */

	mpz_init_set_d (modulus, w->k);
	mpz_mul_2exp (modulus, modulus, w->n);
	mpz_add_si (modulus, modulus, w->c);

/* Calculate prp_base^power mod k*b^n+c */

	mpz_init_set_si (tmp, power);
	mpz_init_set_ui (prp_base_power, prp_base);
	mpz_powm (prp_base_power, prp_base_power, tmp, modulus);

/* Copy the giant value to tmp.  Multiply by prp_base_power to get the final result */

	gtompz (v, tmp);
	mpz_mul (tmp, tmp, prp_base_power);
	mpz_mod (tmp, tmp, modulus);
	mpztog (tmp, v);

/* Cleanup and return */

	mpz_clear (tmp);
	mpz_clear (prp_base_power);
	mpz_clear (modulus);
}

/* Compare the final giant in a PRP run.  Different PRP residue types check for different final values */

int isPRPg (
	giant	v,			/* Final result of PRP powering */
	giant	N,			/* Number we are PRPing, (k*b^n+c)/known_factors */
	struct work_unit *w,		/* Number being tested */
	unsigned int prp_base,		/* PRP base */
	int	prp_residue_type)	/* Type of PRP test performed */
{
	mpz_t	mpz_v, mpz_N, compare_val;
	int	result;

/* Standard Fermat PRP, test for one */

	if (prp_residue_type == PRIMNET_PRP_TYPE_FERMAT) return (isone (v));

/* SPRP test is PRP if result is one or minus one */

	if (prp_residue_type == PRIMNET_PRP_TYPE_SPRP) {
		if (isone (v)) return (TRUE);
		subg (v, N); result = isone (N); addg (v, N);
		return (result);
	}

/* Convert giants to mpz_t type */

	mpz_init (mpz_v);
	gtompz (v, mpz_v);
	mpz_init (mpz_N);
	gtompz (N, mpz_N);
	mpz_init (compare_val);

/* Handle the cofactor case.  We calculated v = a^(N*KF-1) mod (N*KF).  We have a PRP if (v mod N) = (a^KF) mod N */

	if (prp_residue_type == PRIMNET_PRP_TYPE_COFACTOR) {
		mpz_t	kbnc, known_factors, reduced_v;
		// Calculate k*b^n+c
		mpz_init (kbnc);
		mpz_ui_pow_ui (kbnc, w->b, w->n);
		mpz_mul_d (kbnc, kbnc, w->k);
		mpz_add_si (kbnc, kbnc, w->c);
		// Known factors = kbnc / N
		mpz_init (known_factors);
		mpz_divexact (known_factors, kbnc, mpz_N);
		mpz_clear (kbnc);
		// Compare val = base^(known_factors - 1) mod N
		mpz_sub_ui (known_factors, known_factors, 1);
		mpz_set_ui (compare_val, prp_base);
		mpz_powm (compare_val, compare_val, known_factors, mpz_N);
		mpz_clear (known_factors);
		// Reduce v mod N  and compare
		mpz_init (reduced_v);
		mpz_mod (reduced_v, mpz_v, mpz_N);
		result = mpz_eq (reduced_v, compare_val);
		mpz_clear (reduced_v);
	}

/* Handle the weird cases -- Fermat and SPRP variants, one of which gpuOwl uses */

	else {
		mpz_t	power;

/* Calculate the compare_value */

		mpz_init (power);
		if (prp_residue_type == PRIMNET_PRP_TYPE_FERMAT_VAR) mpz_set_si (power, - (w->c - 1));
		else mpz_set_si (power, - (w->c - 1) / 2);
		mpz_set_ui (compare_val, prp_base);
		mpz_powm (compare_val, compare_val, power, mpz_N);
		mpz_clear (power);

/* Now do the comparison(s) */

		result = mpz_eq (mpz_v, compare_val);
		if (prp_residue_type == PRIMNET_PRP_TYPE_SPRP_VAR && !result) {
			mpz_sub (compare_val, mpz_N, compare_val);	// Negate compare val
			result = mpz_eq (mpz_v, compare_val);
		}
	}

/* Cleanup and return */

	mpz_clear (mpz_v);
	mpz_clear (mpz_N);
	mpz_clear (compare_val);
	return (result);
}


/* Do a PRP test */

int prp (
	int	thread_num,		/* Worker thread number */
	struct PriorityInfo *sp_info,	/* SetPriority information */
	struct work_unit *w,		/* Worktodo entry */
	int	pass)			/* PrimeContinue pass */
{
	struct prp_state ps;
	gwhandle gwdata;
	giant	N, exp, tmp;
	int	first_iter_msg, res, stop_reason;
	int	echk, near_fft_limit, sleep5, isProbablePrime;
	int	interim_counter_off_one, interim_mul, mul_final;
	unsigned long explen, final_counter, iters;
	int	slow_iteration_count;
	double	timers[2];
	double	inverse_explen;
	double	reallyminerr = 1.0;
	double	reallymaxerr = 0.0;
	double	best_iteration_time;
	readSaveFileState read_save_file_state; /* Manage savefile names during reading */
	writeSaveFileState write_save_file_state; /* Manage savefile names during writing */
	char	filename[32];
	char	buf[400], JSONbuf[2000], fft_desc[100], res64[17];
	unsigned long last_counter = 0xFFFFFFFF;/* Iteration of last error */
	int	maxerr_recovery_mode = 0;	/* Big roundoff err rerun */
	double	last_suminp = 0.0;
	double	last_sumout = 0.0;
	double	last_maxerr = 0.0;
	double	allowable_maxerr, output_frequency, output_title_frequency;
	char	string_rep[80];
	int	string_rep_truncated;
	int	error_count_messages;
	unsigned long restart_error_count = 0;	/* On a restart, use this error count rather than the one from a save file */
	long	restart_counter = -1;		/* On a restart, this specifies how far back to rollback save files */

/* Null gwnums and giants in case they get freed */

	memset (&ps, 0, sizeof (ps));
	N = exp = NULL;

/* See if this number needs P-1 factoring.  We treat P-1 factoring */
/* that is part of a PRP test as priority work done in pass 1 or as */
/* regular work done in pass 2 if WellBehavedWork or SequentialWorkTodo */
/* is set.  The only way we can get to pass 3 and P-1 still needs to be */
/* done is if pfactor returned STOP_NOT_ENOUGH_MEM on an earlier pass. */
/* In that case, skip onto doing the PRP test until more memory becomes */
/* available. */

	if (w->work_type == WORK_PRP && w->tests_saved > 0.0 && pass != 3) {
		int	pass_to_pfactor;

		pass_to_pfactor = (WELL_BEHAVED_WORK || SEQUENTIAL_WORK) ? 2 : 1;
		if (pass != pass_to_pfactor) return (0);

		stop_reason = pfactor (thread_num, sp_info, w);
		if (stop_reason) return (stop_reason);
	}

/* Done with pass 1 priority work.  Return to do more priority work. */

	if (pass == 1) return (0);

/* Figure out which FFT size we should use */

	stop_reason = pick_fft_size (thread_num, w);
	if (stop_reason) return (stop_reason);

/* Make sure the first-time user runs a successful self-test. */
/* The one-hour self-test may have been useful when it was first introduced */
/* but I think it now does little to catch buggy machines (they eventually */
/* work OK for an hour) and does create user confusion and annoyance. */

#ifdef ONE_HOUR_SELF_TEST
	stop_reason = selfTest (thread_num, sp_info, w);
	if (stop_reason) return (stop_reason);
#endif

/* If the Primenet server has not specified a PRP base (for double-checking), then by default we */
/* mimic LLR's PRP tests using PRP base 3, though we can support bases other than 3. */

	if (w->prp_base) ps.prp_base = w->prp_base;
	else ps.prp_base = IniGetInt (INI_FILE, "PRPBase", 3);

/* If the Primenet server has not specified a PRP residue type (for double-checking), then by default we */
/* to return a standard Fermat PRP residue on k*b^n+c (i.e. calculate base^(k*b^n+c-1)) */

	if (w->prp_residue_type) ps.residue_type = w->prp_residue_type;
	else ps.residue_type = IniGetInt (INI_FILE, "PRPResidueType", PRIMNET_PRP_TYPE_COFACTOR);
	if (w->known_factors == NULL && ps.residue_type == PRIMNET_PRP_TYPE_COFACTOR) ps.residue_type = PRIMNET_PRP_TYPE_FERMAT;

/* Below is some pseudo-code to show how various input numbers are handled for each PRP residue type (rt=residue type, */
/* a=PRP base, E is number for the binary exponentiation code, KF=known factors).  We can do Gerbicz error checking if b=2 and */
/* there are a long string of squarings -- which also greatly reduces the number of mul-by-small-consts when c<0. */
/*
   k * 2^n + c
	if rt=1,5 E=k*2^n, gerbicz after a^k, mul interim by a^-1 if c<0, mul final by a^(c-1), compare to 1
	if rt=2   E=k*2^(n-1), gerbicz after a^k, mul interim by a^-1 if c<0, mul final by a^((c-1)/2), compare to +/-1
	if rt=3   E=k*2^n, gerbicz after a^k, mul interim by a^-1 if c<0, compare to a^-(c-1)
	if rt=4   E=k*2^(n-1), gerbicz after a^k, mul interim by a^-1 if c<0, compare to +/-a^-((c-1)/2)

   (k * 2^n + c) / KF
	if rt=1-4 go to general case
	if rt=5   E=k*2^n, gerbicz after a^k, mul interim by a^-1 if c<0, mul final by a^(c-1), compare to a^(KF-1) mod (N/KF)

   (k * b^n + c) / KF
	if rt=1   E=(k*b^n+c)/KF-1, compare to 1
	if rt=2   E=((k*b^n+c)/KF-1)/2, compare to +/-1
	if rt=3   E=(k*b^n+c)/KF+1, compare to a^2			(pointless case, make rt=1)
	if rt=4   E=((k*b^n+c)/KF+1)/2, compare to +/-a			(pointless case, make rt=2)
	if rt=5   E=k*b^n+c-1, compare mod (N/KF) to a^(KF-1)
*/

/* Set flag if we will perform power-of-two optimizations.  These optimizations reduce the number of mul-by-small constants */
/* by computing a^(k*2^n) which gives us a long run of simple squarings.  These squarings let us do Gerbicz error checking. */

	if (IniGetInt (INI_FILE, "PRPStraightForward", 0))
		ps.two_power_opt = FALSE;
	else
		ps.two_power_opt = (w->b == 2 && (w->known_factors == NULL || ps.residue_type == PRIMNET_PRP_TYPE_COFACTOR));
	if (!ps.two_power_opt) {
		if (ps.residue_type == PRIMNET_PRP_TYPE_FERMAT_VAR) ps.residue_type = PRIMNET_PRP_TYPE_FERMAT;
		else if (ps.residue_type == PRIMNET_PRP_TYPE_SPRP_VAR) ps.residue_type = PRIMNET_PRP_TYPE_SPRP;
	}

/* If k=1,b=2 and we are doing a traditional Fermat PRP implementation, then interim residues are output based on the bit */
/* representation of N = 2^n + c - 1.  N looks like this:
	c >= 0:		100000000000000000000000000ccc
	c < 0:		 11111111111111111111111111ccc
   Our implementation is always going to do binary exponentiation on 2^n.  Looking like this:
			100000000000000000000000000000
   Note that when c<0 we have increased the bit length of N by one bit, which makes our interim residues counter off by one
   from what the user is expecting.  We also need to divide interim residues by the PRP base when c<0. */

	interim_counter_off_one = (ps.two_power_opt && w->k == 1.0 && w->c < 0);
	interim_mul = (ps.two_power_opt && w->c < 0);

/* Flag the PRP tests that require multiplying the final a^exp to account for c */

	if (ps.two_power_opt && (ps.residue_type == PRIMNET_PRP_TYPE_FERMAT || ps.residue_type == PRIMNET_PRP_TYPE_COFACTOR))
		mul_final = w->c - 1;
	else if (ps.two_power_opt && ps.residue_type == PRIMNET_PRP_TYPE_SPRP)
		mul_final = (w->c - 1) / 2;
	else
		mul_final = 0;

/* Determine what highly-reliable error-checking will be done (if any) */

	echk = IniGetInt (INI_FILE, "PRPErrorChecking", 1);
	if (echk == 0) ps.error_check_type = PRP_ERRCHK_NONE;
	if (echk == 1) ps.error_check_type = ps.two_power_opt ? PRP_ERRCHK_GERBICZ : PRP_ERRCHK_NONE;
	if (echk == 2) ps.error_check_type = ps.two_power_opt ? PRP_ERRCHK_GERBICZ : PRP_ERRCHK_DBLCHK;

/* Init the FFT code for squaring modulo k*b^n+c */

begin:	gwinit (&gwdata);
	gwsetmaxmulbyconst (&gwdata, ps.prp_base);
	if (IniGetInt (LOCALINI_FILE, "UseLargePages", 0)) gwset_use_large_pages (&gwdata);
	if (HYPERTHREAD_LL) {
		sp_info->normal_work_hyperthreads = IniGetInt (LOCALINI_FILE, "HyperthreadLLcount", CPU_HYPERTHREADS);
		gwset_will_hyperthread (&gwdata, sp_info->normal_work_hyperthreads);
	}
	gwset_bench_cores (&gwdata, NUM_CPUS);
	gwset_bench_workers (&gwdata, NUM_WORKER_THREADS);
	if (ERRCHK) gwset_will_error_check (&gwdata);
	else gwset_will_error_check_near_limit (&gwdata);
	gwset_num_threads (&gwdata, CORES_PER_TEST[thread_num] * sp_info->normal_work_hyperthreads);
	gwset_thread_callback (&gwdata, SetAuxThreadPriority);
	gwset_thread_callback_data (&gwdata, sp_info);
	gwset_minimum_fftlen (&gwdata, w->minimum_fftlen);
	gwset_safety_margin (&gwdata, IniGetFloat (INI_FILE, "ExtraSafetyMargin", 0.0));
	res = gwsetup (&gwdata, w->k, w->b, w->n, w->c);

/* If we were unable to init the FFT code, then print an error message */
/* and return an error code. */

	if (res) {
		char	string_rep[80];
		gw_as_string (string_rep, w->k, w->b, w->n, w->c);
		sprintf (buf, "PRP cannot initialize FFT code for %s, errcode=%d\n", string_rep, res);
		OutputBoth (thread_num, buf);
		gwerror_text (&gwdata, res, buf, sizeof (buf) - 1);
		strcat (buf, "\n");
		OutputBoth (thread_num, buf);
		if (res == GWERROR_TOO_SMALL) return (STOP_WORK_UNIT_COMPLETE);
		return (STOP_FATAL_ERROR);
	}

/* Compute the number we are testing. */

	stop_reason = setN (&gwdata, thread_num, w, &N);
	if (stop_reason) goto exit;

/* If N is one, the number is already fully factored.  Print an error message. */

	if (isone (N)) {
		sprintf (buf, "PRP test of one is not allowed.  Input string was: %s\n", string_rep);
		OutputBoth (thread_num, buf);
		stop_reason = STOP_WORK_UNIT_COMPLETE;
		goto exit;
	}

/* If N is divisible by the PRP base, then basemulg will crash calculating the modular inverse. */
/* Catch this condition here (before running a full PRP test). */

	{
		mpz_t	tmp;
		int	is_divisible;

		mpz_init (tmp);
		gtompz (N, tmp);
		is_divisible = mpz_divisible_ui_p (tmp, ps.prp_base);
		mpz_clear (tmp);
		if (is_divisible) {
			sprintf (buf, "PRP test of %s aborted -- number is divisible by %u\n", gwmodulo_as_string (&gwdata), ps.prp_base);
			OutputBoth (thread_num, buf);
			stop_reason = STOP_WORK_UNIT_COMPLETE;
			goto exit;
		}
	}

/* Record the amount of memory being used by this thread.  Assume error-checking uses double even */
/* though it really doesn't change the working set all that much. */

	if (ps.error_check_type == PRP_ERRCHK_NONE)
		set_memory_usage (thread_num, 0, cvt_gwnums_to_mem (&gwdata, 1));
	else
		set_memory_usage (thread_num, 0, cvt_gwnums_to_mem (&gwdata, 1) * 2);

/* Allocate memory for the PRP test */

	ps.x = gwalloc (&gwdata);
	if (ps.x == NULL) {
		OutputStr (thread_num, "Error allocating memory for FFT data.\n");
		stop_reason = STOP_OUT_OF_MEM;
		goto exit;
	}
	if (ps.error_check_type == PRP_ERRCHK_GERBICZ || ps.error_check_type == PRP_ERRCHK_DBLCHK) {
		ps.alt_x = gwalloc (&gwdata);
		if (ps.alt_x == NULL) {
			OutputStr (thread_num, "Error allocating memory for error checking.\n");
			stop_reason = STOP_OUT_OF_MEM;
			goto exit;
		}
	}
	if (ps.error_check_type == PRP_ERRCHK_GERBICZ) {
		ps.u0 = gwalloc (&gwdata);
		if (ps.u0 == NULL) {
			OutputStr (thread_num, "Error allocating memory for Gerbicz error checking.\n");
			stop_reason = STOP_OUT_OF_MEM;
			goto exit;
		}
		ps.d = gwalloc (&gwdata);
		if (ps.d == NULL) {
			OutputStr (thread_num, "Error allocating memory for Gerbicz error checking.\n");
			stop_reason = STOP_OUT_OF_MEM;
			goto exit;
		}
	}

/* Format the string representation of the test number */

	strcpy (string_rep, gwmodulo_as_string (&gwdata));
	if (w->known_factors == NULL)
		string_rep_truncated = FALSE;
	else if (strlen (w->known_factors) < 40) {
		char	*p;
		strcat (string_rep, "/");
		strcat (string_rep, w->known_factors);
		while ((p = strchr (string_rep, ',')) != NULL) *p = '/';
		string_rep_truncated = FALSE;
	} else {
		strcat (string_rep, "/known_factors");
		string_rep_truncated = TRUE;
	}

/* Init the title */

	sprintf (buf, "PRP %s", string_rep);
	title (thread_num, buf);

/* Loop reading from save files (and backup save files).  Limit number of backup */
/* files we try to read in case there is an error deleting bad save files. */

	tempFileName (w, filename);
	readSaveFileStateInit (&read_save_file_state, thread_num, filename);
	writeSaveFileStateInit (&write_save_file_state, filename, NUM_JACOBI_BACKUP_FILES);
	for ( ; ; ) {

/* If there are no more save files, start off with the 1st PRP squaring. */

		if (! saveFileExists (&read_save_file_state)) {
			/* If there were save files, they are all bad.  Report a message */
			/* and temporarily abandon the work unit.  We do this in hopes that */
			/* we can successfully read one of the bad save files at a later time. */
			/* This sounds crazy, but has happened when OSes get in a funky state. */
			if (read_save_file_state.a_non_bad_save_file_existed ||
			    (pass == 3 && read_save_file_state.a_save_file_existed)) {
				OutputBoth (thread_num, ALLSAVEBAD_MSG);
				return (0);
			}
			/* No save files existed, start from scratch. */
			ps.counter = 0;
			ps.error_count = 0;
			first_iter_msg = FALSE;
			break;
		}

/* Read a PRP save file.  If successful, then if we've rolled back far enough for a restart break out of loop. */

		if (readPRPSaveFile (&gwdata, read_save_file_state.current_filename, w, &ps)) {
			if (restart_counter < 0 || ps.counter <= (unsigned long) restart_counter) {
				first_iter_msg = TRUE;
				break;
			} else {
				// Don't treat save files we are ignoring due to a restart as bad.
				read_save_file_state.a_non_bad_save_file_existed = 0;
				_unlink (read_save_file_state.current_filename);
			}
		}

/* On read error, output message and loop to try the next backup save file. */

		else
			saveFileBad (&read_save_file_state);
	}

/* If this is a restart from an error, use the incremented error_count in restart_error_count */
/* rather than the error_count from a save file. */

	if (restart_error_count) ps.error_count = restart_error_count;

/* Output a message saying we are starting/resuming the PRP test. */
/* Also output the FFT length. */

	gwfft_description (&gwdata, fft_desc);
	strcpy (buf, (ps.counter == 0) ? "Starting " : "Resuming ");
	if (ps.error_check_type == PRP_ERRCHK_GERBICZ) sprintf (buf+strlen(buf), "Gerbicz error-checking ");
	if (ps.error_check_type == PRP_ERRCHK_DBLCHK) sprintf (buf+strlen(buf), "double-checking ");
	if (ps.prp_base != 3) sprintf (buf+strlen(buf), "%u-", ps.prp_base);
	sprintf (buf+strlen(buf), "PRP test of %s using %s\n", string_rep, fft_desc);
	OutputStr (thread_num, buf);

/* Calculate the exponent we will use to do our left-to-right binary exponentiation */

	exp = allocgiant ((((unsigned long) (w->n * _log2 (w->b))) >> 5) + 5);
	if (exp == NULL) {
		stop_reason = OutOfMemory (thread_num);
		goto exit;
	}

/* As a small optimization, base 2 numbers are computed as a^(k*2^n) or a^(k*2^(n-1)) mod N with the final result multiplied by */
/* a^(c-1).  This eliminates tons of mul-by-consts at the expense of lots of bookkeepping headaches and one squaring if k=1 and c<0. */

	if (ps.two_power_opt) {
		int	gerbicz_squarings;
		if (ps.residue_type == PRIMNET_PRP_TYPE_FERMAT ||
		    ps.residue_type == PRIMNET_PRP_TYPE_FERMAT_VAR ||
		    ps.residue_type == PRIMNET_PRP_TYPE_COFACTOR)
			gerbicz_squarings = w->n;
		else
			gerbicz_squarings = w->n - 1;
		ultog (2, exp);
		power (exp, gerbicz_squarings);
		dblmulg (w->k, exp);
	}

/* Standard PRP test.  Subtract 1 from N to compute a^(N-1) mod N */

	else {
		gtog (N, exp);
		iaddg (-1, exp);
	}

/* Get the exact bit length of the binary exponent.  We will perform bitlen(exp)-1 squarings for the PRP test. */

	explen = bitlen (exp);
	final_counter = explen - 1;

/* Hyperthreading backoff is an option to pause the program when iterations */
/* take longer than usual.  This is useful on hyperthreaded machines so */
/* that prime95 doesn't steal cycles from a foreground task, thus hurting */
/* the computers responsiveness. */

	best_iteration_time = 1.0e50;
	slow_iteration_count = 0;

/* Clear all timers */

	clear_timers (timers, sizeof (timers) / sizeof (timers[0]));

/* Init vars for Test/Status and CommunicateWithServer */

	strcpy (w->stage, "PRP");
	inverse_explen = 1.0 / (double) final_counter;
	w->pct_complete = (double) ps.counter * inverse_explen;
	calc_output_frequencies (&gwdata, &output_frequency, &output_title_frequency);

/* If we are near the maximum exponent this fft length can test, then we */
/* will error check all iterations */

	near_fft_limit = exponent_near_fft_limit (&gwdata);

/* Figure out the maximum round-off error we will allow.  By default this is 27/64 when near the FFT limit and 26/64 otherwise. */
/* We've found that this default catches errors without raising too many spurious error messages.  We let the user override */
/* this default for user "Never Odd Or Even" who tests exponents well beyond an FFT's limit.  He does his error checking by */
/* running the first-test and double-check simultaneously. */

	allowable_maxerr = IniGetFloat (INI_FILE, "MaxRoundoffError", (float) (near_fft_limit ? 0.421875 : 0.40625));

/* Set the proper starting value and state if no save file was present */

	if (ps.counter == 0) {
		/* For Mersenne numbers we support FFT data shifting */
		if (w->k == 1.0 && w->b == 2 && w->n > 1000 && w->c == -1) {
			unsigned long word, bit_in_word;
			// Generate a random initial shift count
			srand ((unsigned) time (NULL));
			ps.units_bit = (rand () << 16) + rand ();
			if (CPU_FLAGS & CPU_RDTSC) { uint32_t hi,lo; rdtsc(&hi,&lo); ps.units_bit += lo; }
			// Let user override random initial shift count
			ps.units_bit = IniGetInt (INI_FILE, "InitialShiftCount", ps.units_bit);
			// Initial shift count can't be larger than n-64 (the -64 avoids wraparound in setting intial value)
			ps.units_bit = ps.units_bit % (w->n - 64);
			// Perform the initial shift, putting at most 24-bits in a word (should be safe)
			dbltogw (&gwdata, 0.0, ps.x);
			bitaddr (&gwdata, ps.units_bit, &word, &bit_in_word);
			set_fft_value (&gwdata, ps.x, word, (ps.prp_base << bit_in_word) & 0xFFFFFF);
			set_fft_value (&gwdata, ps.x, word+1, ps.prp_base >> (24 - bit_in_word));
		} else {
			ps.units_bit = 0;
			dbltogw (&gwdata, (double) ps.prp_base, ps.x);
		}

/* The easy state case is no high-reliability error-checking */

		if (ps.error_check_type == PRP_ERRCHK_NONE) {
			ps.state = PRP_STATE_NORMAL;
			ps.start_counter = 0;			// Value not used
			ps.end_counter = 0;			// Value not used
		}

/* The next easiest case is double-the-work error-checking comparing residues at specified intervals */
	
		if (ps.error_check_type == PRP_ERRCHK_DBLCHK) {
			ps.state = PRP_STATE_DCHK_PASS1;
			ps.start_counter = 0;
			ps.end_counter = IniGetInt (INI_FILE, "PRPDoublecheckCompareInterval", 100000);
			if (ps.end_counter > final_counter) ps.end_counter = final_counter;
			if (ps.units_bit == 0) {
				gwcopy (&gwdata, ps.x, ps.alt_x);
				ps.alt_units_bit = 0;
			} else {
				gwadd3 (&gwdata, ps.x, ps.x, ps.alt_x);
				ps.alt_units_bit = ps.units_bit + 1;
				if (ps.alt_units_bit >= w->n) ps.alt_units_bit -= w->n;
			}
		}

/* The final case of high-reliability error-checking is Gerbicz error checking, described below. */
/* See http://mersenneforum.org/showthread.php?t=22471 for background information. */
/* In a nutshell, if PRPing k*2^n+c we calculate (prp_base^k)^(2^n) which ends with a long string of squarings. */
/* Let u[0] = (prp_base^k), our n squarings are defined as:
	u[i]=u[0]^(2^i) mod mp, for i=0..n
   We define a "checksum" function below, which is updated every L-th squaring:
	d[t]=prod(i=0,t,u[L*i]) mod mp
   The key idea is that the checksum function can be calculated two different ways, thus the two can be compared to detect an error:
	d[t]=d[t-1]*u[L*t] mod mp
	d[t]=u[0]*d[t-1]^(2^L) mod mp
   The larger L we choose, the lower the error-checking overhead cost.  For L of 1000, we catch errors every 1 million iterations
   with an overhead of just 0.2%. */

		if (ps.error_check_type == PRP_ERRCHK_GERBICZ) {
			// We first compute (prp_base^k) by double-checking 
			if (w->k != 1.0) {
				ps.state = PRP_STATE_DCHK_PASS1;
				ps.start_counter = 0;
				ps.end_counter = (int) _log2 (w->k);
				gwcopy (&gwdata, ps.x, ps.alt_x);
				ps.alt_units_bit = ps.units_bit;
			} else {
				ps.state = PRP_STATE_GERB_START_BLOCK;
			}
		}
	}

/* Get setting for verbosity of hardware error messages.  Force output of "confidence is excellent" when error checking. */

	error_count_messages = IniGetInt (INI_FILE, "ErrorCountMessages", 3);
	if (ps.error_check_type != PRP_ERRCHK_NONE) error_count_messages |= 0x8000;

/* Do the PRP test */

//#define CHECK_ITER
#ifdef CHECK_ITER
{giant t1, t2;
t1 = popg (&gwdata.gdata, ((unsigned long) gwdata.bit_length >> 4) + 5);
t2 = popg (&gwdata.gdata, ((unsigned long) gwdata.bit_length >> 4) + 5);
gwtogiant (&gwdata, ps.x, t1);
rotateg (t1, w->n, ps.units_bit, &gwdata.gdata);
#endif
	gwsetmulbyconst (&gwdata, ps.prp_base);
	iters = 0;
	while (ps.counter < final_counter) {
		gwnum	x;			/* Pointer to number to square */
		unsigned long *units_bit;	/* Pointer to units_bit to update */
		int	saving, saving_highly_reliable, sending_residue, interim_residue, interim_file;
		int	actual_frequency;

/* If this is the first iteration of a Gerbicz error-checking block, then */
/* determine "L" -- the number of squarings between each Gerbicz multiplication */
/* We end this Gerbicz block after L^2 iterations.  */
/* If there aren't many iterations left, revert to simple double-checking. */

		if (ps.state == PRP_STATE_GERB_START_BLOCK) {
			int	iters_left = final_counter - ps.counter;
			if (iters_left < 49) {
				ps.state = PRP_STATE_DCHK_PASS1;
				ps.start_counter = ps.counter;
				ps.end_counter = final_counter;
				gwcopy (&gwdata, ps.x, ps.alt_x);
				ps.alt_units_bit = ps.units_bit;
			} else {
				int	gerbicz_block_size;
				ps.state = PRP_STATE_GERB_MID_BLOCK;
				gerbicz_block_size = (int) (IniGetFloat (INI_FILE, "PRPGerbiczCompareIntervalAdj", 1.0) *
							    IniGetInt (INI_FILE, "PRPGerbiczCompareInterval", 1000000));
				if (iters_left < gerbicz_block_size) gerbicz_block_size = iters_left;
				ps.L = (unsigned long) sqrt ((double) gerbicz_block_size);
				ps.start_counter = ps.counter;
				ps.next_mul_counter = ps.counter + ps.L;
				ps.end_counter = ps.counter + ps.L * ps.L;
				gwcopy (&gwdata, ps.x, ps.u0);
				gwcopy (&gwdata, ps.x, ps.d);
			}
		}

/* Save if we are stopping, right after we pass an errored iteration, several iterations before retesting */
/* an errored iteration so that we don't have to backtrack very far to do a gwsquare_carefully iteration */
/* (we don't do the iteration immediately before because a save operation may change the FFT data and make */
/* the error non-reproducible), and finally save if the save file timer has gone off. */

		stop_reason = stopCheck (thread_num);
		saving = stop_reason || ps.counter == last_counter-8 || ps.counter == last_counter || testSaveFilesFlag (thread_num);
		saving_highly_reliable = FALSE;

/* Round off error check the first and last 50 iterations, before writing a save file, near an FFT size's limit, */
/* or check every iteration option is set, and every 128th iteration. */

		echk = ps.counter < 50 || ps.counter >= final_counter-50 || saving || near_fft_limit || ERRCHK || ((ps.counter & 127) == 0);
		gw_clear_maxerr (&gwdata);

/* Check if we should send residue to server, output residue to screen, or create an interediate save file */
/* Beware that if we are PRPing a Mersenne number our iteration numbers are off by one compared to other */
/* PRP programs, we adjust accordingly. */

		sending_residue = (ps.state == PRP_STATE_NORMAL || ps.state == PRP_STATE_DCHK_PASS1 || ps.state == PRP_STATE_GERB_MID_BLOCK) &&
				  ps.counter > 0 &&
				  ((ps.counter+1-interim_counter_off_one) == 500000 ||
				   ((ps.counter+1-interim_counter_off_one) % 5000000 == 0 && IniGetInt (INI_FILE, "SendInterimResidues", 1)));
		interim_residue = INTERIM_RESIDUES &&
				  (ps.state == PRP_STATE_NORMAL || ps.state == PRP_STATE_DCHK_PASS1 || ps.state == PRP_STATE_GERB_MID_BLOCK) &&
				  (ps.counter > 0 && (ps.counter+1-interim_counter_off_one) % INTERIM_RESIDUES == 0);
		interim_file = INTERIM_FILES &&
			       (ps.state == PRP_STATE_NORMAL || ps.state == PRP_STATE_DCHK_PASS2 || ps.state == PRP_STATE_GERB_MID_BLOCK) &&
			       (ps.counter > 0 && (ps.counter+1) % INTERIM_FILES == 0);

/* Do one PRP iteration */

		timers[1] = 0.0;
		start_timer (timers, 1);

/* If we are doing one of the Gerbicz multiplies (not a squaring), then handle that here */

		if (ps.state == PRP_STATE_GERB_MID_BLOCK_MULT) {
			gwstartnextfft (&gwdata, 0);		/* Do not start next forward FFT */
			gwsetnormroutine (&gwdata, 0, 1, 0);	/* Always roundoff error check multiplies */
			gwsafemul (&gwdata, ps.x, ps.d);	/* "Safe" multiply that does not change ps.x */
			x = ps.d;				/* Set pointer for checking roundoff errors, sumouts, etc. */
		} else if (ps.state == PRP_STATE_GERB_END_BLOCK_MULT) {
			gwstartnextfft (&gwdata, 0);		/* Do not start next forward FFT */
			gwsetnormroutine (&gwdata, 0, 1, 0);	/* Always roundoff error check multiplies */
			gwmul (&gwdata, ps.u0, ps.alt_x);	/* Multiply (u0 value can be destroyed) */
			x = ps.alt_x;				/* Set pointer for checking roundoff errors, sumouts, etc. */
		}

/* Otherwise, do a squaring iteration */

		else {

/* Use state to decide which number we are squaring */

			if (ps.state == PRP_STATE_NORMAL || ps.state == PRP_STATE_DCHK_PASS1 || ps.state == PRP_STATE_GERB_MID_BLOCK) {
				x = ps.x;
				units_bit = &ps.units_bit;
			} else {			// (ps.state == PRP_STATE_DCHK_PASS2 || ps.state == PRP_STATE_GERB_END_BLOCK) {
				x = ps.alt_x;
				units_bit = &ps.alt_units_bit;
			}

/* Decide if we can start the next forward FFT.  This is faster, but leaves the result in an "unsavable-to-disk" state. */

			gwstartnextfft (&gwdata,
					!saving && !maxerr_recovery_mode && ps.counter != ps.end_counter-1 &&
					ps.counter > 35 && ps.counter < explen-35 &&
					!sending_residue && !interim_residue && !interim_file);

/* Process this bit.  Use square carefully the first and last 30 iterations. */
/* This should avoid any pathological non-random bit pattterns.  Also square */
/* carefully during an error recovery. This will protect us from roundoff */
/* errors up to (1.0 - 0.40625). */

#ifdef CHECK_ITER
squareg (t1);
if (bitval (exp, final_counter-ps.counter-1)) ulmulg (ps.prp_base, t1);
specialmodg (&gwdata, t1);
if (w->known_factors) modg (N, t1);
gwstartnextfft (&gwdata, 0);
echk=1;
#endif
			if (bitval (exp, final_counter-ps.counter-1)) {
				gwsetnormroutine (&gwdata, 0, echk, 1);
			} else {
				gwsetnormroutine (&gwdata, 0, echk, 0);
			}
			if (maxerr_recovery_mode && ps.counter == last_counter) {
				gwsquare_carefully (&gwdata, x);
				maxerr_recovery_mode = 0;
				echk = 0;
			} else if (ps.counter < 30 || ps.counter >= final_counter-30)
				gwsquare_carefully (&gwdata, x);
			else
				gwsquare (&gwdata, x);

			*units_bit <<= 1;
			if (*units_bit >= w->n) *units_bit -= w->n;

#ifdef CHECK_ITER
gwtogiant (&gwdata, ps.x, t2);
rotateg (t2, w->n, ps.units_bit, &gwdata.gdata);
if (w->known_factors) modg (N, t2);
if (gcompg (t1, t2) != 0)
OutputStr (thread_num, "Iteration failed.\n");
//if (ps.counter == 100) ps.counter = final_counter-1;
#endif
		}

// introduce an error every random # iterations when debugging highly reliable error checking
//#define INTRODUCE_ERRORS
#ifdef INTRODUCE_ERRORS
		if ((rand () & 0x7FFF) == 134)  // one out of 32768
			*x += 5.0;
#endif
		
/* End iteration timing and increase count of iterations completed */

		end_timer (timers, 1);
		timers[0] += timers[1];
		iters++;

/* Update min/max round-off error */

		if (echk) {
			if (ps.counter > 30 && gw_get_maxerr (&gwdata) < reallyminerr) reallyminerr = gw_get_maxerr (&gwdata);
			if (gw_get_maxerr (&gwdata) > reallymaxerr) reallymaxerr = gw_get_maxerr (&gwdata);
		}

/* If the sum of the output values is an error (such as infinity) then raise an error. */

		if (gw_test_illegal_sumout (&gwdata)) {
			sprintf (buf, ERRMSG0, ps.counter+1, final_counter, ERRMSG1A);
			OutputBoth (thread_num, buf);
			inc_error_count (2, &ps.error_count);
			if (ps.error_check_type == PRP_ERRCHK_NONE) {
				sleep5 = TRUE;
				goto restart;
			}
		}

/* Check that the sum of the input numbers squared is approximately */
/* equal to the sum of unfft results.  Since this check may not */
/* be perfect, check for identical results after a restart. */

		if (gw_test_mismatched_sums (&gwdata)) {
			if (ps.counter == last_counter &&
			    gwsuminp (&gwdata, x) == last_suminp &&
			    gwsumout (&gwdata, x) == last_sumout) {
				OutputBoth (thread_num, ERROK);
				inc_error_count (3, &ps.error_count);
				gw_clear_error (&gwdata);
			} else {
				char	msg[100];
				sprintf (msg, ERRMSG1B, gwsuminp (&gwdata, x), gwsumout (&gwdata, x));
				sprintf (buf, ERRMSG0, ps.counter+1, final_counter, msg);
				OutputBoth (thread_num, buf);
				inc_error_count (0, &ps.error_count);
				if (ps.error_check_type == PRP_ERRCHK_NONE) {
					last_counter = ps.counter;
					last_suminp = gwsuminp (&gwdata, x);
					last_sumout = gwsumout (&gwdata, x);
					sleep5 = TRUE;
					goto restart;
				}
			}
		}

/* Check for excessive roundoff error.  If round off is too large, repeat */
/* the iteration to see if this was a hardware error.  If it was repeatable */
/* then repeat the iteration using a safer, slower method.  This can */
/* happen when operating near the limit of an FFT. */

		if (echk && gw_get_maxerr (&gwdata) > allowable_maxerr) {
			if (ps.counter == last_counter && gw_get_maxerr (&gwdata) == last_maxerr) {
				OutputBoth (thread_num, ERROK);
				inc_error_count (3, &ps.error_count);
				gw_clear_error (&gwdata);
				OutputBoth (thread_num, ERRMSG5);
				maxerr_recovery_mode = 1;
				sleep5 = FALSE;
				goto restart;
			} else {
				char	msg[100];
				sprintf (msg, ERRMSG1C, gw_get_maxerr (&gwdata), allowable_maxerr);
				sprintf (buf, ERRMSG0, ps.counter+1, final_counter, msg);
				OutputBoth (thread_num, buf);
				inc_error_count (1, &ps.error_count);
				if (ps.error_check_type == PRP_ERRCHK_NONE) {
					last_counter = ps.counter;
					last_maxerr = gw_get_maxerr (&gwdata);
					sleep5 = FALSE;
					goto restart;
				}
			}
		}

/* Update counter, percentage complete */

		ps.counter++;
		w->pct_complete = (double) ps.counter * inverse_explen;
		if (ps.error_check_type == PRP_ERRCHK_DBLCHK) {
			unsigned long true_counter;
			true_counter = ps.start_counter + ((ps.counter - ps.start_counter) >> 1);
			if (ps.state == PRP_STATE_DCHK_PASS2) true_counter += ((ps.end_counter - ps.start_counter) >> 1);
			w->pct_complete = (double) true_counter * inverse_explen;
		}

/* Output the title every so often */

		actual_frequency = (int) (ITER_OUTPUT * output_title_frequency);
		if (actual_frequency < 1) actual_frequency = 1;
		if (ps.counter % actual_frequency == 0 || first_iter_msg) {
			sprintf (buf, "%.*f%% of %s", (int) PRECISION, trunc_percent (w->pct_complete), string_rep);
			title (thread_num, buf);
		}

/* Print a message every so often */

		actual_frequency = (int) (ITER_OUTPUT * output_frequency);
		if (actual_frequency < 1) actual_frequency = 1;
		if ((ps.counter % actual_frequency == 0 && ps.state != PRP_STATE_GERB_MID_BLOCK_MULT &&
		     ps.state != PRP_STATE_GERB_END_BLOCK && ps.state != PRP_STATE_GERB_END_BLOCK_MULT) || first_iter_msg) {
			sprintf (buf, "Iteration: %ld / %ld [%.*f%%]", ps.counter, final_counter, (int) PRECISION, trunc_percent (w->pct_complete));
			/* Append a short form total errors message */
			if ((error_count_messages & 0xFF) == 1)
				make_error_count_message (ps.error_count, error_count_messages, buf + strlen (buf),
							  (int) (sizeof (buf) - strlen (buf)));
			/* Truncate first message */
			if (first_iter_msg) {
				strcat (buf, ".\n");
				clear_timer (timers, 0);
				first_iter_msg = FALSE;
			}
			/* In v28.5 and later, format a consise message including the ETA */
			else if (!CLASSIC_OUTPUT) {
				double speed;
				/* Append roundoff error */
				if ((OUTPUT_ROUNDOFF || ERRCHK) && reallymaxerr >= 0.001) {
					sprintf (buf+strlen(buf), ", roundoff: %5.3f", reallymaxerr);
					if (!CUMULATIVE_ROUNDOFF) reallyminerr = 1.0, reallymaxerr = 0.0;
				}
				/* Append ms/iter */
				speed = timer_value (timers, 0) / (double) iters;
				sprintf (buf+strlen(buf), ", ms/iter: %6.3f", speed * 1000.0);
				clear_timer (timers, 0);
				iters = 0;
				/* Append ETA */
				formatETA ((final_counter - ps.counter) * speed, buf+strlen(buf));
				strcat (buf, "\n");
			}
			/* Format the classic (pre-v28.5) message */
			else {
				/* Append optional roundoff message */
				if (ERRCHK && ps.counter > 30) {
					sprintf (buf+strlen(buf), ".  Round off: %10.10f to %10.10f", reallyminerr, reallymaxerr);
					if (!CUMULATIVE_ROUNDOFF) reallyminerr = 1.0, reallymaxerr = 0.0;
				}
				if (CUMULATIVE_TIMING) {
					strcat (buf, ".  Total time: ");
					print_timer (timers, 0, buf, TIMER_NL);
				} else {
					strcat (buf, ".  Per iteration time: ");
					divide_timer (timers, 0, iters);
					print_timer (timers, 0, buf, TIMER_NL | TIMER_CLR);
					iters = 0;
				}
			}
			OutputStr (thread_num, buf);

/* Output a verbose message showing the error counts.  This way a user is likely to */
/* notice a problem without reading the results.txt file. */

			if ((error_count_messages & 0xFF) >= 2 &&
			    make_error_count_message (ps.error_count, error_count_messages, buf, sizeof (buf)))
				OutputStr (thread_num, buf);
		}

/* Print a results file message every so often */

		if ((ps.counter % ITER_OUTPUT_RES == 0 && ps.state != PRP_STATE_GERB_MID_BLOCK_MULT &&
		     ps.state != PRP_STATE_GERB_END_BLOCK && ps.state != PRP_STATE_GERB_END_BLOCK_MULT) || (NO_GUI && stop_reason)) {
			sprintf (buf, "Iteration %ld / %ld\n", ps.counter, final_counter);
			writeResults (buf);
		}

/* If double-checking, at end of pass 1 rollback counter and start computing alt_x. */
/* If double-checking, at end of pass 2 compare values and move onto next block. */

		if (ps.state == PRP_STATE_DCHK_PASS1 && ps.counter == ps.end_counter) {
			ps.state = PRP_STATE_DCHK_PASS2;
			ps.counter = ps.start_counter;
		}
		if (ps.state == PRP_STATE_DCHK_PASS2 && ps.counter == ps.end_counter) {
			if (!areTwoPRPValsEqual (&gwdata, w->n, ps.x, ps.units_bit, ps.alt_x, ps.alt_units_bit)) {
				sprintf (buf, ERRMSG6, ps.start_counter);
				OutputBoth (thread_num, buf);
				inc_error_count (7, &ps.error_count);
				restart_counter = ps.start_counter;		/* rollback to this iteration */
				sleep5 = FALSE;
				goto restart;
			}
			/* If doing a full double-check, start next block of iterations */
			if (ps.error_check_type == PRP_ERRCHK_DBLCHK) {
				ps.state = PRP_STATE_DCHK_PASS1;
				ps.start_counter = ps.counter;
				ps.end_counter = ps.start_counter + IniGetInt (INI_FILE, "PRPDoublecheckCompareInterval", 100000);
				if (ps.end_counter > final_counter) ps.end_counter = final_counter;
			}
			/* Otherwise, we're doing a Gerbicz error-check.  Start next Gerbicz block. */
			else {
				ps.state = PRP_STATE_GERB_START_BLOCK;
			}
			/* We've reached a verified iteration, create a save file and mark it highly reliable. */
			/* But if there are less than 1000 iterations left, don't bother creating the save file. */
			if (final_counter - ps.counter >= 1000) {
				saving = TRUE;
				saving_highly_reliable = TRUE;
			}
		}

/* If Gerbicz error-checking, see if this is an L-th iteration that needs to do a checksum multiply. */
/* Also check if this is the L^2-th iteration where we switch to an alternate method of computing the checksum. */
/* NOTE: We do not need to worry about shift_counts in the checksum, both checksums will end up with the same shift count. */

		if (ps.state == PRP_STATE_GERB_MID_BLOCK && ps.counter == ps.next_mul_counter) {
			// At block's end, copy d to alt_x in preparation for alternate method of computing Gerbicz checksum
			if (ps.counter == ps.end_counter) gwcopy (&gwdata, ps.d, ps.alt_x);
			// Back counter up by one and do one multiply in the computation of first Gerbicz checksum value
			ps.state = PRP_STATE_GERB_MID_BLOCK_MULT;
			ps.counter -= 1;
		}
		else if (ps.state == PRP_STATE_GERB_MID_BLOCK_MULT) {
			// If in middle of Gerbicz block, do another "L" squarings
			if (ps.counter != ps.end_counter) {
				ps.state = PRP_STATE_GERB_MID_BLOCK;
				ps.next_mul_counter += ps.L;
			}
			// At block's end, rollback counter to do "L" squarings of alt_x to create second Gerbicz checksum value
			else {
				ps.state = PRP_STATE_GERB_END_BLOCK;
				ps.counter -= ps.L;
			}
		}

/* If Gerbicz error-checking and we've finished computing the alternate checksum, then compare the two checksums */
/* and start next Gerbicz block. */

		if (ps.state == PRP_STATE_GERB_END_BLOCK && ps.counter == ps.end_counter) {
			ps.state = PRP_STATE_GERB_END_BLOCK_MULT;
			ps.counter -= 1;
		}
		else if (ps.state == PRP_STATE_GERB_END_BLOCK_MULT) {
			double	gerbicz_block_size_adjustment;
			// We adjust the compare interval size downward when errors occur and upwards when they dont.
			// That way buggy machines will lose fewer iterations when rolling back.
			gerbicz_block_size_adjustment = IniGetFloat (INI_FILE, "PRPGerbiczCompareIntervalAdj", 1.0);
			// compare alt_x, d (the two Gerbicz checksum values that must match)
			if (!areTwoPRPValsEqual (&gwdata, w->n, ps.alt_x, 0, ps.d, 0)) {
				sprintf (buf, ERRMSG7, ps.start_counter);
				OutputBoth (thread_num, buf);
				gerbicz_block_size_adjustment *= 0.25;		/* This will halve next L */
				if (gerbicz_block_size_adjustment < 0.01) gerbicz_block_size_adjustment = 0.01;
				IniWriteFloat (INI_FILE, "PRPGerbiczCompareIntervalAdj", (float) gerbicz_block_size_adjustment);
				inc_error_count (7, &ps.error_count);
				restart_counter = ps.start_counter;		/* rollback to this iteration */
				sleep5 = FALSE;
				goto restart;
			}
			gerbicz_block_size_adjustment *= 1.0473;		/* 30 good blocks to double L */
			if (gerbicz_block_size_adjustment > 1.0) gerbicz_block_size_adjustment = 1.0;
			IniWriteFloat (INI_FILE, "PRPGerbiczCompareIntervalAdj", (float) gerbicz_block_size_adjustment);
			/* Start next Gerbicz block */
			ps.state = PRP_STATE_GERB_START_BLOCK;
			/* We've reached a verified iteration, create a save file and mark it highly reliable. */
			/* But if there are less than 1000 iterations left, don't bother creating the save file. */
			if (final_counter - ps.counter >= 1000) {
				saving = TRUE;
				saving_highly_reliable = TRUE;
			}
		}

/* Write results to a file every DISK_WRITE_TIME minutes */

		if (saving) {
			if (! writePRPSaveFile (&gwdata, &write_save_file_state, w, &ps)) {
				sprintf (buf, WRITEFILEERR, filename);
				OutputBoth (thread_num, buf);
			}
			// Mark save files that contain verified computations.  This will keep the save file
			// for a longer period of time (i.e. will not be replaced by a save file that does
			// not also contain verified computations).
			if (saving_highly_reliable) write_save_file_state.special = 1;
		}

/* If an escape key was hit, write out the results and return */

		if (stop_reason) {
			sprintf (buf, "Stopping PRP test of %s at iteration %ld [%.*f%%]\n",
				 string_rep, ps.counter, (int) PRECISION, trunc_percent (w->pct_complete));
			OutputStr (thread_num, buf);
			goto exit;
		}

/* Send the 64-bit residue to the server at specified interims.  The server will record */
/* the residues for possible verification at a later date.  We could catch suspect computers */
/* or malicious cheaters without doing a full double-check. */

		if (sending_residue) {
			struct primenetAssignmentProgress pkt;
			memset (&pkt, 0, sizeof (pkt));
			strcpy (pkt.computer_guid, COMPUTER_GUID);
			pkt.cpu_num = thread_num;
			strcpy (pkt.assignment_uid, w->assignment_uid);
			strcpy (pkt.stage, w->stage);
			pkt.pct_complete = w->pct_complete * 100.0;
			pkt.end_date = (unsigned long) work_estimate (thread_num, w);
			pkt.next_update = (uint32_t) (DAYS_BETWEEN_CHECKINS * 86400.0);
			pkt.fftlen = w->fftlen;
			pkt.iteration = ps.counter;
			tmp = popg (&gwdata.gdata, ((unsigned long) gwdata.bit_length >> 5) + 5);
			gwtogiant (&gwdata, x, tmp);
			rotateg (tmp, w->n, *units_bit, &gwdata.gdata);
			if (interim_mul) basemulg (tmp, w, ps.prp_base, -1);
			if (w->known_factors && ps.residue_type != PRIMNET_PRP_TYPE_COFACTOR) modg (N, tmp);
			sprintf (pkt.residue, "%08lX%08lX", (unsigned long) tmp->n[1], (unsigned long) tmp->n[0]);
			sprintf (pkt.error_count, "%08lX", ps.error_count);
			spoolMessage (-PRIMENET_ASSIGNMENT_PROGRESS, &pkt);
			pushg (&gwdata.gdata, 1);
		}

/* Output the 64-bit residue at specified interims. */

		if (interim_residue) {
			tmp = popg (&gwdata.gdata, ((unsigned long) gwdata.bit_length >> 5) + 5);
			gwtogiant (&gwdata, x, tmp);
			rotateg (tmp, w->n, *units_bit, &gwdata.gdata);
			if (interim_mul) basemulg (tmp, w, ps.prp_base, -1);
			if (w->known_factors && ps.residue_type != PRIMNET_PRP_TYPE_COFACTOR) modg (N, tmp);
			sprintf (buf, "%s interim Wg%d residue %08lX%08lX at iteration %ld\n",
				 string_rep, PORT, (unsigned long) tmp->n[1], (unsigned long) tmp->n[0],
				 ps.counter - interim_counter_off_one);
			OutputBoth (thread_num, buf);
			pushg (&gwdata.gdata, 1);
		}

/* Write a save file every INTERIM_FILES iterations. */

		if (interim_file) {
			char	interimfile[32];
			writeSaveFileState state;
			sprintf (interimfile, "%s.%03ld", filename, ps.counter / INTERIM_FILES);
			writeSaveFileStateInit (&state, interimfile, 0);
			state.num_ordinary_save_files = 99;
			writePRPSaveFile (&gwdata, &state, w, &ps);
		}

/* If ten iterations take 40% longer than a typical iteration, then */
/* assume a foreground process is running and sleep for a short time */
/* to give the foreground process more CPU time.  Even though a foreground */
/* process runs at higher priority, hyperthreading will cause this */
/* program to run at an equal priority, hurting responsiveness. */

		if (HYPERTHREADING_BACKOFF && explen > 1500000) {
			if (timers[1] < best_iteration_time)
				best_iteration_time = timers[1];
			if (timers[1] > 1.40 * best_iteration_time) {
				if (slow_iteration_count == 10) {
					sprintf (buf, "Pausing %lu seconds.\n", HYPERTHREADING_BACKOFF);
					OutputStr (thread_num, buf);
					Sleep (HYPERTHREADING_BACKOFF * 1000);
				}
				slow_iteration_count++;
			} else
				slow_iteration_count = 0;
		}
	}
#ifdef CHECK_ITER
pushg(&gwdata.gdata, 2);}
#endif

/* Free up some memory */

	gwfree (&gwdata, ps.alt_x);
	gwfree (&gwdata, ps.u0);
	gwfree (&gwdata, ps.d);

/* See if we've found a probable prime.  If not, format a 64-bit residue. */

	tmp = popg (&gwdata.gdata, ((unsigned long) gwdata.bit_length >> 5) + 5);
	gwtogiant (&gwdata, ps.x, tmp);
	rotateg (tmp, w->n, ps.units_bit, &gwdata.gdata);
	if (mul_final) basemulg (tmp, w, ps.prp_base, mul_final);
	if (w->known_factors && ps.residue_type != PRIMNET_PRP_TYPE_COFACTOR) modg (N, tmp);
	isProbablePrime = isPRPg (tmp, N, w, ps.prp_base, ps.residue_type);
	if (!isProbablePrime) sprintf (res64, "%08lX%08lX", (unsigned long) tmp->n[1], (unsigned long) tmp->n[0]);
	pushg (&gwdata.gdata, 1);
	gwfree (&gwdata, ps.x);
			
/* Print results */

	if (isProbablePrime) {
		sprintf (buf, "%s is a probable prime", string_rep);
		if (ps.prp_base != 3) sprintf (buf+strlen(buf), " (%u-PRP)", ps.prp_base);
		strcat (buf, "!");
	} else {
		sprintf (buf, "%s is not prime.  ", string_rep);
		if (ps.prp_base != 3) sprintf (buf+strlen(buf), "Base-%u ", ps.prp_base);
		if (ps.residue_type != PRIMNET_PRP_TYPE_FERMAT) sprintf (buf+strlen(buf), "Type-%d ", ps.residue_type);
		sprintf (buf+strlen(buf), "RES64: %s.", res64);
	}
	sprintf (buf+strlen(buf), " Wg%d: %08lX,", PORT, SEC1 (w->n));
	if (ps.units_bit) sprintf (buf+strlen(buf), "%ld,", ps.units_bit);
	sprintf (buf+strlen(buf), "%08lX\n", ps.error_count);
	OutputStr (thread_num, buf);
	formatMsgForResultsFile (buf, w);

/* Update the output file */

	if ((isProbablePrime && IniGetInt (INI_FILE, "OutputPrimes", 0)) ||
	    (!isProbablePrime && IniGetInt (INI_FILE, "OutputComposites", 0)))
		writeResults (buf);

/* Print known factors */

	if (string_rep_truncated) {
		char	*bigbuf;
		bigbuf = (char *) malloc (strlen (w->known_factors) + 100);
		if (bigbuf != NULL) {
			sprintf (bigbuf, "Known factors used for PRP test were: %s\n", w->known_factors);
			OutputStr (thread_num, bigbuf);
			if ((isProbablePrime && IniGetInt (INI_FILE, "OutputPrimes", 0)) ||
			    (!isProbablePrime && IniGetInt (INI_FILE, "OutputComposites", 0)))
				writeResults (bigbuf);
			free (bigbuf);
		}
	}

/* Format a JSON version of the result.  An example from gpuowl follows: */
/* {"exponent":25000000, "worktype":"PRP-3", "status":"C", "res64":"0000000000000000", */
/* "residue-checksum":"c6b0b26c", "program":{"name":"gpuowl", "version":"1.1"}, "timestamp":"2017-09-04 23:28:16", */
/* "errors":{"gerbicz":0}, "user":"meme", "cpu":"Vega [Radeon RX Vega] 64 @83:0.0", "aid":"FF00AA00FF00AA00FF00AA00FF00AA00"} */

	sprintf (JSONbuf, "{\"status\":\"%s\"", isProbablePrime ? "P" : "C");
	if (w->k == 1.0 && w->b == 2 && w->c == -1)
		sprintf (JSONbuf+strlen(JSONbuf), ", \"exponent\":%lu", w->n);
	else
		sprintf (JSONbuf+strlen(JSONbuf), ", \"k\":%.0f, \"b\":%lu, \"n\":%lu, \"c\":%ld", w->k, w->b, w->n, w->c);
	if (w->known_factors != NULL) {
		char	fac_string[1210];
		if (strlen (w->known_factors) <= 1200) strcpy (fac_string, w->known_factors);
		else {
			memcpy (fac_string, w->known_factors, 1200);
			strcpy (fac_string+1200, "...");
		}
		sprintf (JSONbuf+strlen(JSONbuf), ", \"known-factors\":\"%s\"", fac_string);
	}
	sprintf (JSONbuf+strlen(JSONbuf), ", \"worktype\":\"PRP-%u\"", ps.prp_base);
	if (!isProbablePrime) {
		sprintf (JSONbuf+strlen(JSONbuf), ", \"res64\":\"%s\"", res64);
		sprintf (JSONbuf+strlen(JSONbuf), ", \"residue-type\":%d", ps.residue_type);
	}
	sprintf (JSONbuf+strlen(JSONbuf), ", \"fft-length\":%lu", gwdata.FFTLEN);
	if (ps.units_bit) sprintf (JSONbuf+strlen(JSONbuf), ", \"shift-count\":%ld", ps.units_bit);
	sprintf (JSONbuf+strlen(JSONbuf), ", \"error-code\":\"%08lX\"", ps.error_count);
	sprintf (JSONbuf+strlen(JSONbuf), ", \"security-code\":\"%08lX\"", SEC1(w->n));
	sprintf (JSONbuf+strlen(JSONbuf), ", \"program\":{\"name\":\"Prime95\", \"version\":\"%s\", \"build\":%s, \"port\":%d}",
		 VERSION, BUILD_NUM, PORT);
	{
		time_t rawtime;
		time (&rawtime);
		strftime (JSONbuf+strlen(JSONbuf), 80, ", \"timestamp\":\"%Y-%m-%d %H:%M:%S\"", gmtime (&rawtime));
	}
	if (ps.error_check_type == PRP_ERRCHK_DBLCHK)
		sprintf (JSONbuf+strlen(JSONbuf), ", \"errors\":{\"dblchk\":%lu}", (ps.error_count >> 20) & 0xF);
	if (ps.error_check_type == PRP_ERRCHK_GERBICZ)
		sprintf (JSONbuf+strlen(JSONbuf), ", \"errors\":{\"gerbicz\":%lu}", (ps.error_count >> 20) & 0xF);
	if (USERID[0]) sprintf (JSONbuf+strlen(JSONbuf), ", \"user\":\"%s\"", USERID);
	if (COMPID[0]) sprintf (JSONbuf+strlen(JSONbuf), ", \"computer\":\"%s\"", COMPID);
	if (w->assignment_uid[0]) sprintf (JSONbuf+strlen(JSONbuf), ", \"aid\":\"%s\"", w->assignment_uid);
	sprintf (JSONbuf+strlen(JSONbuf), "}\n");
	if (IniGetInt (INI_FILE, "OutputJSON", 1)) writeResults (JSONbuf);

/* Output results to the server */

	{
		struct primenetAssignmentResult pkt;
		memset (&pkt, 0, sizeof (pkt));
		strcpy (pkt.computer_guid, COMPUTER_GUID);
		strcpy (pkt.assignment_uid, w->assignment_uid);
		strcpy (pkt.message, buf);
		pkt.result_type = isProbablePrime ? PRIMENET_AR_PRP_PRIME : PRIMENET_AR_PRP_RESULT;
		pkt.k = w->k;
		pkt.b = w->b;
		pkt.n = w->n;
		pkt.c = w->c;
		strcpy (pkt.residue, res64);
		sprintf (pkt.error_count, "%08lX", ps.error_count);
		pkt.prp_base = ps.prp_base;
		pkt.prp_residue_type = ps.residue_type;
		pkt.shift_count = ps.units_bit;
		pkt.num_known_factors = (w->known_factors == NULL) ? 0 : countCommas (w->known_factors) + 1;
		pkt.gerbicz = (ps.error_check_type != PRP_ERRCHK_NONE);
		pkt.fftlen = gwfftlen (&gwdata);
		pkt.done = TRUE;
		strcpy (pkt.JSONmessage, JSONbuf);
		spoolMessage (PRIMENET_ASSIGNMENT_RESULT, &pkt);
	}

/* Delete the continuation files. */

	unlinkSaveFiles (&write_save_file_state);

/* Output good news to the screen in an infinite loop */

	if (isProbablePrime && !SILENT_VICTORY_PRP) {
		gwthread thread_handle;
		char	*arg;
		arg = (char *) malloc (strlen (string_rep) + 1);
		strcpy (arg, string_rep);
		gwthread_create (&thread_handle, &good_news_prp, (void *) arg);
	}

/* Return work unit completed stop reason */

	stop_reason = STOP_WORK_UNIT_COMPLETE;

/* Cleanup and exit */

exit:	gwdone (&gwdata);
	free (N);
	free (exp);
	return (stop_reason);

/* An error occured, output a message saying we are restarting, sleep, */
/* then try restarting at last save point. */

restart:if (sleep5) OutputBoth (thread_num, ERRMSG2);
	OutputBoth (thread_num, ERRMSG3);

/* Save the incremented error count to be used in the restart rather than the error count read from a save file */

	restart_error_count = ps.error_count;

/* Sleep five minutes before restarting */

	if (sleep5) {
		stop_reason = SleepFive (thread_num);
		if (stop_reason) return (stop_reason);
	}

/* Return so that last continuation file is read in */

	gwdone (&gwdata);
	free (N);
	goto begin;
}