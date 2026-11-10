"""Validate generated local links, assets and fragment IDs without network access."""
from pathlib import Path
from html.parser import HTMLParser
from urllib.parse import urlsplit, unquote
import sys
root=Path("site").resolve()
class Page(HTMLParser):
    def __init__(self,text):
        super().__init__();self.ids=set();self.links=[];self.feed(text)
    def handle_starttag(self,tag,attrs):
        a=dict(attrs)
        if "id" in a:self.ids.add(a["id"])
        for attr in ("href","src"):
            if attr in a:self.links.append(a[attr])
pages={p:Page(p.read_text(encoding="utf-8")) for p in root.rglob("*.html")}
errors=[]
for file,page in pages.items():
    if file.name=="404.html":continue
    for link in page.links:
        u=urlsplit(link)
        if u.scheme or u.netloc or link.startswith("/"):continue
        target=(file.parent/unquote(u.path)).resolve() if u.path else file
        if target.is_dir():target=target/"index.html"
        if not target.exists():errors.append(f"{file.relative_to(root)}: missing {link}")
        elif u.fragment and target in pages and unquote(u.fragment) not in pages[target].ids:
            errors.append(f"{file.relative_to(root)}: missing anchor {link}")
for error in errors:print(error)
print(f"Checked {len(pages)} HTML pages; {len(errors)} broken local links/anchors")
sys.exit(bool(errors))
