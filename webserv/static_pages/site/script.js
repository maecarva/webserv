
const imageslinks = [
	"/site/images/hubert.webp",
	"/site/images/jack.webp",
	"/site/images/larmina.webp",
	"/site/images/armand.webp",
	"/site/images/raymond.webp",
]

document.addEventListener('DOMContentLoaded', function() {
	let links = document.querySelectorAll(".imageb");
	links.forEach((link) => {
		link.addEventListener('click', function() {
			document.querySelector("img").src = `/site/images/` + link.id + ".webp"
		})
	})

	let randombutton = document.querySelector(".random")
	randombutton.addEventListener('click', function() {
		let attribute = document.querySelector("img").src
		let newattribute = attribute

		while (newattribute == attribute) {
			newattribute = imageslinks[Math.floor(Math.random() * (imageslinks.length))]
		}
		document.querySelector("img").src = newattribute
	})
});

