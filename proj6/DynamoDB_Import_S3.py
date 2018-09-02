execfile('')

from boto.s3.connection import S3Connection
s3 = S3Connection()
bucket = s3.get_bucket('sgcs15spproj6tokyo')
from boto.dynamodb2.table import Table
ngram = Table('prjsixresult')                                             # using table name

for s3object in bucket.list():
    if 'output' in s3object.key and 'part-' in s3object.key:              # select input file
        content = s3object.get_contents_as_string()
        for each_line in content.split('\n'):                             # read content
            if each_line.strip() != "":                                   # do not read empty content
                words, counts = each_line.split('\t')
                ngram.put_item(data={'words':words, 'counts': counts})    # put the data
