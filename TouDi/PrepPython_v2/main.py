# Run the script
# ./PrepPython_v2/main.py ./input_files/input_default_valid.csv

import argparse
import heapq
from typing import Any, Dict, List, Optional, Set


SECONDS_IN_A_DAY = 86400
HEADER = "#job_id,runtime_in_seconds,next_job_id"
ERROR_MESSAGE = "Malformed Input"
PRINT_LN = True
PRINT_DETAILS = True


class LineRecord:
    def __init__(self, cur_id: int, dur_secs: int, next_id: int):
        self.cur_id = cur_id
        self.dur_secs = dur_secs
        self.next_id = next_id


class Job:
    def __init__(self, cur_id: int, dur_secs: int = -1, next_id: int = -1) -> None:
        self.id = cur_id
        self.dur_secs = dur_secs
        self.next_id = next_id
        self.pre_job = None
        self.next_job = None

    def setJobInfo(self, line_record: LineRecord, next_job: Any) -> None:
        self.dur_secs = line_record.dur_secs
        self.next_id = line_record.next_id
        self.next_job = next_job
        if next_job is not None:
            self.next_job.pre_job = self

    def isTheLastJob(self) -> bool:
        return self.dur_secs >= 0 and self.next_job is None


class Chain:
    def __init__(self, init_job: Job):
        self.init_id = init_job.id
        self.end_id = 0
        self.last_id = 0
        self.dur_secs = 0
        self.num_jobs = 0
        job = init_job
        while True:
            self.dur_secs += job.dur_secs
            self.num_jobs += 1
            if job.next_job is None:
                self.last_id = job.id
                if job.next_id > 0:
                    self.end_id = self.last_id
                break
            else:
                job = job.next_job

    def isChainEnded(self) -> bool:
        return self.end_id > 0

    def isDurRangeValid(self) -> bool:
        return self.dur_secs <= SECONDS_IN_A_DAY

    def _intToString(self, val: int) -> str:
        val_str = str(val)
        if len(val_str) == 1:
            val_str = "0"+val_str
        return val_str

    def formatDurationString(self, dur_secs: int) -> str:
        hrs = dur_secs//3600
        mins = (dur_secs % 3600)//60
        secs = dur_secs % 60
        return "{}:{}:{}".format(
            self._intToString(hrs),
            self._intToString(mins),
            self._intToString(secs)
        )


def raiseError(print_ln: bool, ln: int) -> int:
    if print_ln:
        print(ERROR_MESSAGE+f": {ln}")
    else:
        print(ERROR_MESSAGE)

    return 1


def lineIsValid(line: str) -> Optional[LineRecord]:
    elements = line.split(',')
    if len(elements) != 3:
        return None

    for ele in elements:
        if not ele.isdigit():
            # Return True if all characters in the string are digits
            # and there is at least one character
            return None

        if len(ele) > 1 and ele[0] == '0':
            return None

    if elements[0] == "0":  # Current job id should not be 0
        return None

    if int(elements[1]) > SECONDS_IN_A_DAY:  # Job duration is too long
        return None

    return LineRecord(*[int(ele) for ele in elements])


def processLineRecordToJob(
    line_record: LineRecord,
    map_id_to_job: Dict[int, Job]
) -> bool:
    cur_id = line_record.cur_id
    next_id = line_record.next_id

    next_job = None
    if next_id in map_id_to_job:
        next_job = map_id_to_job[next_id]
        if (next_job.pre_job is not None):
            if PRINT_DETAILS:
                # print(map_id_to_job.keys())
                print(
                    f"The next job already had the dependency, error. {next_id}")
            return False
    elif next_id > 0:
        next_job = Job(cur_id=next_id)
        map_id_to_job[next_id] = next_job

    if cur_id in map_id_to_job:
        cur_job = map_id_to_job[cur_id]
        if (cur_job.next_job is not None) or cur_job.isTheLastJob():
            if PRINT_DETAILS:
                print(
                    f"The job previously started another job or it "
                    "was the last job of a chain, error. {cur_id}"
                )
            return False
    else:
        cur_job = Job(
            cur_id=line_record.cur_id,
            dur_secs=line_record.dur_secs,
            next_id=line_record.next_id
        )
        map_id_to_job[cur_id] = cur_job

    # Will set the bi-dir links b/w cur_job and next_job
    cur_job.setJobInfo(line_record=line_record, next_job=next_job)

    return True


def isAChainValid(init_job: Job, set_job_ids: Set[int]) -> bool:
    job = init_job
    while job is not None:
        if job.id in set_job_ids:
            if PRINT_DETAILS:
                print(
                    f"The job_id was visited before so that it is not a chain. {job.id}")
            return False
        set_job_ids.add(job.id)
        job = job.next_job

    return True


def checkJobCyclesAndReturnChainHeads(map_id_to_job: Dict[int, Job]) -> Optional[List[Job]]:
    set_job_ids = set()
    ls_chain_heads = []
    for _, job in map_id_to_job.items():
        if job.pre_job is None:
            # No dependency. The beginning of a chain.
            # print(job.id)
            if isAChainValid(job, set_job_ids):
                ls_chain_heads.append(job)
            else:
                # print(set_job_ids)
                return None

    if len(set_job_ids) != len(map_id_to_job):
        if PRINT_DETAILS:
            print("There are some cycles.")
            for job_id in map_id_to_job.keys():
                if job_id not in set_job_ids:
                    print(job_id)
        return None

    return ls_chain_heads


def genChains(map_id_to_job: Dict[int, Job]) -> Optional[List[Chain]]:
    ls_chain_heads = checkJobCyclesAndReturnChainHeads(map_id_to_job)

    if ls_chain_heads is None:
        if PRINT_DETAILS:
            print("The parsed log has job cycles.")
        return None

    ls_chains = []
    for chain_head in ls_chain_heads:
        chain = Chain(chain_head)
        if not chain.isDurRangeValid():
            if PRINT_DETAILS:
                print("The parsed chain has too long duration.")
            return None
        ls_chains.append(chain)

    return ls_chains


def printSummaryReport(ls_chains: List[Chain]):
    pqchains = [(-chain.dur_secs, chain) for chain in ls_chains]
    heapq.heapify(pqchains)

    while len(pqchains) > 0:
        _, chain = heapq.heappop(pqchains)
        print("-")
        print(f"start_job: {chain.init_id}")
        print(f"last_job: {chain.last_id}")
        print(f"number_of_jobs: {chain.num_jobs}")
        print(
            f"job_chain_runtime: {chain.formatDurationString(chain.dur_secs)}")
        print(
            f"average_job_time: {chain.formatDurationString(chain.dur_secs//chain.num_jobs)}")

    print("-")


def main(fpath: str):
    map_id_to_job = {}

    ln = 0
    try:
        with open(fpath, 'r') as myfile:
            lines = myfile.read().splitlines()
    except Exception:
        return raiseError(PRINT_LN, ln)
    ln += 1

    if len(lines) == 0 or lines[0] != HEADER:
        return raiseError(PRINT_LN, ln)
    else:
        ln += 1

    for line in lines[1:]:
        line_record = lineIsValid(line)
        if line_record is None:
            return raiseError(PRINT_LN, ln)

        if not processLineRecordToJob(line_record, map_id_to_job):
            if PRINT_DETAILS:
                print(f"{ln}: {line}")
            return raiseError(PRINT_LN, ln)

        ln += 1

    ls_chains = genChains(map_id_to_job)

    if ls_chains is None:
        return raiseError(PRINT_LN, -1)

    printSummaryReport(ls_chains)

    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("fpath")
    args = parser.parse_args()
    fpath = args.fpath

    # fpath = "../input_files/input_valid_default.csv"

    main(fpath)
