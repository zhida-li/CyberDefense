
import urllib.request
import concurrent.futures

from datetime import date, timedelta
from pathlib import Path
from html.parser import HTMLParser

import gconfig


oneday = timedelta(days=1)

class RIPE:

    baseUrl = "https://data.ris.ripe.net"

    def __init__(self, workdir):
        self.workdir = workdir
        self.directory_data = {}

    def get_directory_data(collector, date):
        url = f"{RIPE.baseUrl}/{collector}/{date.year}.{date.month:0>2}"
        page_data = urllib.request.urlopen(url).read().decode()
        parser = RipeFilesParser()
        parser.feed(page_data)
        return parser.getfiles()

    def get_filelist(self, collector, date):
        if collector not in self.directory_data:
            filelist = RIPE.get_directory_data(collector, date)
            self.directory_data[collector] = { date.year : { date.month : filelist } }
        elif date.year not in self.directory_data[collector]:
            filelist = RIPE.get_directory_data(collector, date)
            self.directory_data[collector][date.year] = { date.mont: filelist }
        elif date.month not in self.directory_data[collector][date.year]:
            filelist = RIPE.get_directory_data(collector, date)
            self.directory_data[collector][date.year][date.month] = filelist

        return self.directory_data[collector][date.year][date.month]

    def download_single_file(self, url, workdir, fpath):
        data = urllib.request.urlopen(url).read()
        outfd = open(f"{workdir}/{fpath}","wb")
        outfd.write(data)
        outfd.close()

    def download_day(self, collector, date):
        files = self.get_filelist(collector, date)
        Path(f"{self.workdir}/ripe/{collector}/{date.year}.{date.month:0>2}.{date.day:0>2}").mkdir(parents=True, exist_ok=True)
        nameprefix = f"updates.{date.year}{date.month:0>2}{date.day:0>2}"

        tp = concurrent.futures.ThreadPoolExecutor(max_workers=gconfig.param("download_threads", 6))
        for f in files:
            if f.startswith(nameprefix):
                url = f"{RIPE.baseUrl}/{collector}/{date.year:0>2}.{date.month:0>2}/{f}"
                tp.submit(self.download_single_file, url, self.workdir, f"/ripe/{collector}/{date.year}.{date.month:0>2}.{date.day:0>2}/{f}")
        tp.shutdown(wait=True)

    def download_days(self, collector, startday, endday):
        if startday <= endday:
            curday = startday
            while curday <= endday:
                self.download_day(collector, curday)
                curday = curday + oneday

class RipeFilesParser(HTMLParser):
    def __init__(self):
        self.files = []
        super(RipeFilesParser,self).__init__()

    def handle_starttag(self, tag, attrs):
        if tag == "a":
            for attr in attrs:
                (name,value) = attr
                if name == "href":
                    self.files.append(value)
    def getfiles(self):
        return self.files


if __name__ == "__main__":
    # TODO: Add tests
    print("No tests yet.")