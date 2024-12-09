export default {
  mode: "jit",
  content: ["./src/**/*.{html,js,jsx,ts,tsx}", "./public/index.html"],
  darkMode: "class",
  theme: {
    extend: {
      width: {
        "25vw": "25vw",
        "75vw": "75vw"
      },
      height: {
        "1/4vh": "25vh",
        "1/3vh": "33vh",
        "1/2vh": "50vh",
        "3/4vh": "75vh"
      }
    }
  },
  plugins: []
};
