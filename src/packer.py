import sys
from PIL import Image

class PackNode(object):
	def __init__(self, area):
		#if tuple contains two elements, assume they are width and height, and origin is (0,0)
		if len(area) == 2:
			area = (0,0,area[0],area[1])
		self.area = area

	def get_width(self):
		return self.area[2] - self.area[0]
	width = property(fget=get_width)

	def get_height(self):
		return self.area[3] - self.area[1]
	height = property(fget=get_height)

	def insert(self, area):
		if hasattr(self, 'child'):
			a = self.child[0].insert(area)
			if a is None: return self.child[1].insert(area)
			return a

		area = PackNode(area)
		if area.width <= self.width and area.height <= self.height:
			self.child = [None,None]
			self.child[0] = PackNode((self.area[0]+area.width, self.area[1], self.area[2], self.area[1] + area.height))
			self.child[1] = PackNode((self.area[0], self.area[1]+area.height, self.area[2], self.area[3]))
			return PackNode((self.area[0], self.area[1], self.area[0]+area.width, self.area[1]+area.height))

# chops an image into w by h rectangles
def chop_image(image, size):
	pieces = []

	w = size[0]
	h = size[1]

	cols = image.size[0] // w
	rows = image.size[1] // h

	for y in range(rows):
		for x in range(cols):
			box = (x * w, y * h, (x + 1) * w, (y + 1) * h)
			img = image.crop(box)
			if img.split()[-1].getbbox() is not None:
				pieces.append(img)
	return pieces

def main():
	args		= sys.argv
	im			= Image.open(args[1])
	outfile		= args[2]
	sprite_size	= int(args[3]), int(args[4])
	size		= int(args[5]), int(args[6] or args[5])
	format		= 'RGBA'

	pieces	= chop_image(im, sprite_size)
	bboxes	= [x.split()[-1].getbbox() for x in pieces]
	#pad with extra pixel on left and bottom
	bboxes	= [(x[0], x[1], x[2] + 1, x[3] + 1) for x in bboxes]
	cropped	= [x[0].crop(x[1]) for x in zip(pieces, bboxes)]
	ordered	= sorted(zip(pieces, bboxes, cropped), reverse = True, key = lambda x : x[2].size[0] * x[2].size[1])

	tree	= PackNode(size)
	image	= Image.new(format, size)

	uvs = []

	#insert each image into the PackNode area
	for orig, area, img in ordered:
		uv = tree.insert(img.size)
		if uv is None: raise ValueError('Pack size too small.')
		image.paste(img, uv.area)
		uvs.append(uv)

	image.save(outfile)

	#TODO add lua metadata

if __name__ == "__main__":
	main()