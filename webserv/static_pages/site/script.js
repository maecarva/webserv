
const imageslinks = [
	"/site/hubert.webp",
	"/site/jack.webp",
	"/site/larmina.webp",
	"/site/armand.webp",
	"/site/raymond.webp",
]

document.addEventListener('DOMContentLoaded', function() {
	let links = document.querySelectorAll(".imageb");
	links.forEach((link) => {
		link.addEventListener('click', function() {
			document.querySelector("img").src = `/site/` + link.id + ".webp"
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