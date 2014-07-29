with open("test.bed", "rb") as fh:
    fcontent = fh.read()

bins = ["{:08b}".format(ord(char)) for char in fcontent]
hexs = ["{:02x}".format(ord(char)) for char in fcontent]
