"use strict";

// Screenshot lightbox.
const lightbox = document.querySelector(".lightbox");
const lightboxImage = lightbox.querySelector("img");

document.querySelectorAll(".gallery button").forEach((button) => {
    button.addEventListener("click", () => {
        lightboxImage.src = button.dataset.full;
        lightboxImage.alt = button.textContent.trim();
        lightbox.showModal();
    });
});

lightbox.addEventListener("click", () => lightbox.close());
