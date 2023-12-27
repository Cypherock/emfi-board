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


from . import *
import click
import json
import progressbar
import shutil


@click.group()
def cli():
    pass


@cli.command(help="Show information about traces from a given log file")
@click.argument("log", type=click.File("r"))
def info(log):
    total_size = 0
    trace_count = 0
    existing = 0
    existing_batch_file_count = 0
    batches_already_seen = set()
    lines = log.readlines()
    for line in progressbar.progressbar(lines):
        record = json.loads(line)
        if "bid" in record:
            # Trace file saved in batch
            bid = record["bid"]
            if is_valid_rid(bid):
                bid = bytes.fromhex(bid)
                trace_count += 1
                if bid not in batches_already_seen:
                    batches_already_seen.add(bid)
                    try:
                        size = os.path.getsize(get_trace_path(bid))
                        total_size += size
                        existing_batch_file_count += 1
                    except OSError:
                        pass
        else:
            rid = record["id"]
            if is_valid_rid(rid):
                rid = bytes.fromhex(rid)
                trace_count += 1
                trace_path = get_trace_path(rid)
                try:
                    size = os.path.getsize(trace_path)
                    total_size += size
                    existing += 1
                except OSError as e:
                    pass
    print(f"{len(lines)} records")
    print(f"{trace_count} traces")
    print(f"{existing} trace files found")
    print(f"{existing_batch_file_count} batch files found")
    size_gb = total_size / (1024**3)
    print(f"{size_gb:.3f} GB in traces database")


@cli.command(help="Delete traces recorded in a log file")
@click.argument("log", type=click.File("r"))
def rmtraces(log):
    confirmation = input("Delete traces from log? (y/n) ")
    if confirmation != "y":
        return
    total_size = 0
    count = 0
    lines = log.readlines()
    for line in progressbar.progressbar(lines):
        record = json.loads(line)
        if "bid" in record:
            # Trace file saved in batch
            rid = record["bid"]
        else:
            rid = record["id"]
        if is_valid_rid(rid):
            trace_path = get_trace_path(bytes.fromhex(rid))
            try:
                size = os.path.getsize(trace_path)
                os.remove(trace_path)
                count += 1
                total_size += size
            except OSError as e:
                pass
    print(f"{count} files removed")
    size_gb = total_size / (1024**3)
    print(f"{size_gb:.3f} GB freed")


def copy(fid: bytes, from_db: str, to_db: str) -> Optional[int]:
    from_trace_dir, from_filename = get_trace_dir_and_filename(fid, from_db)
    from_path = os.path.join(from_trace_dir, from_filename)
    to_trace_dir, to_filename = get_trace_dir_and_filename(fid, to_db)
    to_path = os.path.join(to_trace_dir, to_filename)
    if os.path.exists(from_path):
        size = os.path.getsize(os.path.join(from_trace_dir, from_filename))
        if not os.path.exists(to_trace_dir):
            os.makedirs(to_trace_dir)
        shutil.copy(from_path, to_path)
        return size


@cli.command(help="Copy traces from a database to another")
@click.argument("log", type=click.File("r"))
@click.argument("from_db")
@click.argument("to_db")
def cp(log, from_db, to_db):
    total_size = 0
    trace_count = 0
    existing = 0
    existing_batch_file_count = 0
    batches_already_seen = set()
    lines = log.readlines()
    for line in progressbar.progressbar(lines):
        record = json.loads(line)
        if "bid" in record:
            # Trace file saved in batch
            bid = record["bid"]
            if is_valid_rid(bid):
                bid = bytes.fromhex(bid)
                trace_count += 1
                if bid not in batches_already_seen:
                    batches_already_seen.add(bid)
                    size = copy(bid, from_db, to_db)
                    if size is not None:
                        total_size += size
                        existing_batch_file_count += 1
        else:
            rid = record["id"]
            if is_valid_rid(rid):
                rid = bytes.fromhex(rid)
                trace_count += 1
                size = copy(rid, from_db, to_db)
                if size is not None:
                    total_size += size
                    existing += 1
    print(f"{len(lines)} records")
    print(f"{trace_count} traces")
    print(f"{existing} trace copied")
    print(f"{existing_batch_file_count} batch files copied")
    size_gb = total_size / (1024**3)
    print(f"{size_gb:.3f} GB copied")


if __name__ == "__main__":
    cli()
