import { createRoot } from "react-dom/client";
const App = lazy(() => (process.env.BASE_ENV === "prod" ? import("./prod") : import("./dev")));
import "./styles.less";
import React, { lazy } from "react";
const container = document.getElementById("root");
const root = createRoot(container as HTMLElement);

root.render(<App />);
