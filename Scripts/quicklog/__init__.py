# This file is part of Quicklog
#
# Quicklog is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#
# Copyright 2023 Ledger SAS, written by Olivier Hériveaux


import json
import os
import random
import datetime
import numpy as np
import re
from typing import Optional


def gen_rand_id() -> bytes:
    """:return: A randomly generated 64-bit identifier"""
    return random.randint(0, 0xFFFFFFFFFFFFFFFF).to_bytes(8, "big")


session_id = gen_rand_id()


def new_record() -> dict:
    """
    :return: new record dict with a generated random Id, the current session
        Id, and a timestamp.
    """
    rec = {
        "session": session_id.hex(),
        "id": gen_rand_id().hex(),
        "timestamp": datetime.datetime.now().timestamp(),
    }
    return rec


def is_valid_rid(rid: str) -> bool:
    """
    :return: True if given string is a valid record id, i.e. it is a lowercase
        hexadecimal string with at least 10 characters.

    This method is used as a sanity check since record ids are used to create
    files and folders in the traces database.
    """
    return (len(rid) >= 10) and (re.fullmatch(r"[0-9a-f]+", rid) is not None)


def get_trace_dir_and_filename(rid: bytes, traces_dir: Optional[str] = None) -> str:
    """
    Returns trace directory and filename corresponding to the given id.

    :param rid: Record or batch identifier
    :param traces_dir: Traces database directory. If None, `TRACESDIR` environment
        variable is used.
    """
    if traces_dir is None:
        traces_dir = os.environ["TRACESDIR"]
    return (
        os.path.join(
            traces_dir,
            f"{rid[0]:02x}",
            f"{rid[1]:02x}",
            f"{rid[2]:02x}",
            f"{rid[3]:02x}",
        ),
        rid[4:].hex() + ".npy",
    )


def get_trace_path(rid: bytes, traces_dir: Optional[str] = None) -> str:
    trace_dir, filename = get_trace_dir_and_filename(rid, traces_dir)
    return os.path.join(trace_dir, filename)


def save_trace(record, trace, sample_rate=None, position=None, delay=None):
    """
    Save given trace in the trace database using given record Id, and set
    trace properties to the current record.

    This creates one trace file for each trace. To group traces in batch files,
    use the class :class:`TraceBatchWriter`.

    :param record: Current experiment record
    :param trace: Trace data
    :param sample_rate: Trace sample rate
    :param position: Acquisition horizontal position
    """
    rid = record["id"]
    assert is_valid_rid(rid)
    trace_dir, filename = get_trace_dir_and_filename(bytes.fromhex(rid))
    if not os.path.exists(trace_dir):
        os.makedirs(trace_dir)
    trace_path = os.path.join(trace_dir, filename)
    if sample_rate is not None:
        record["trace.sample_rate"] = sample_rate
    if position is not None:
        record["trace.position"] = position
    if delay is not None:
        record["trace.delay"] = delay
    np.save(trace_path, trace)


def load_trace(record: dict, traces_dir: Optional[str] = None) -> Optional[np.array]:
    """
    Loads the trace corresponding to a record. Trace can either be saved in a
    single file or in a batch file.

    :param record: Experiment record
    :param traces_dir: Traces database directory. If None, `TRACESDIR` environment
        variable is used.
    :return: Numpy trace, or None if the trace file does not exist.
    """
    if "bid" in record:
        # Trace is saved in a batch file
        trace_dir, filename = get_trace_dir_and_filename(
            bytes.fromhex(record["bid"]), traces_dir
        )
        path = os.path.join(trace_dir, filename)
        if os.path.exists(path):
            with open(os.path.join(trace_dir, filename), "rb") as f:
                f.seek(record["toff"])
                trace = np.load(f)
            return trace
    else:
        # Trace is saved in a single file
        trace_dir, filename = get_trace_dir_and_filename(bytes.fromhex(record["id"]))
        path = os.path.join(trace_dir, filename)
        if os.path.exists(path):
            return np.load(os.path.join(trace_dir, filename))
    return None


class Log:
    def __init__(self, path: str = "log", append=True):
        self.file = open(path, "ab" if append else "wb")

    def append(self, data: dict):
        """
        :param data: Log record data.
        """
        encoded = json.dumps(data)
        self.file.write((encoded + "\n").encode())

    def flush(self):
        """Flush log file to force write on disk."""
        self.file.flush()


def read_log(path: str = "log"):
    """
    Generator which yields everyline of a log file, parsing lines as JSON data.
    """
    for line in open(path, "r"):
        yield json.loads(line)


class TraceBatchWriter:
    """
    Allows storing multiple traces into single files, which can be usefull to increase
    performance of traces analysis, such as side-channel attacks.

    When used, log records will have three more columns:
    - `bid`: Batch ID, randomly generated unique identifier (hex string)
    - `tid`: Trace index in the batch (int)
    - `toff`: Trace offset in the batch file

    In batch files, traces are stored separately as numpy objects. It is not
    stored as a numpy array of traces, to make single trace access possible and
    efficient. The batch file path is built from the `bid` field.

    The trace offset allows loading a single trace from a batch without reading
    all the batch file.

    Everytime a new trace is recorded, it is immediately saved to disk by
    appending the data to the opened batch file.
    """

    def __init__(self, batch_size: int = 1000):
        assert batch_size >= 1
        self.batch_size = batch_size
        self.bid = gen_rand_id()
        self.tid = 0  # Index of next trace in the current batch
        self.file = None

    def save_trace(self, record, trace, sample_rate=None, position=None, delay=None):
        if self.file is None:
            assert self.tid == 0
            trace_dir, filename = get_trace_dir_and_filename(self.bid)
            if not os.path.exists(trace_dir):
                os.makedirs(trace_dir)
            path = os.path.join(trace_dir, filename)
            self.file = open(path, "wb")
        record["bid"] = self.bid.hex()
        record["tid"] = self.tid
        record["toff"] = self.file.tell()  # Trace offset in the file
        if sample_rate is not None:
            record["trace.sample_rate"] = sample_rate
        if position is not None:
            record["trace.position"] = position
        if delay is not None:
            record["trace.delay"] = delay
        np.save(self.file, trace)
        self.tid += 1
        if self.tid == self.batch_size:
            self.file.close()
            self.file = None
            self.bid = gen_rand_id()
            self.tid = 0


class CachedTraceLoader:
    def __init__(self, traces_dir: Optional[str] = None):
        self.current_bid = None
        self.traces = []
        self.traces_dir = traces_dir

    def load_trace(self, record: dict):
        if "bid" in record:
            bid = bytes.fromhex(record["bid"])
            if bid != self.current_bid:
                self.__load_batch(bid)
            tid = record["tid"]
            return self.traces[tid]
        else:
            load_trace(record)

    def __load_batch(self, bid: bytes):
        with open(get_trace_path(bid, self.traces_dir), "rb") as f:
            f.seek(0, os.SEEK_END)
            size = f.tell()
            f.seek(0)
            self.traces.clear()
            while f.tell() < size:
                self.traces.append(np.load(f))
        self.current_bid = bid
