import { createHashRouter, Navigate, RouteObject } from "react-router-dom";
import { lazy, Suspense } from "react";
import React from "react";
import { Spin } from "antd";
const ConfigPage = lazy(() => import("@/pages/ConfigPage"));
const NewCommit = lazy(() => import("@/pages/NewCommit"));
const CommitPage = lazy(() => import("@/pages/CommitPage"));
const IndexPage = lazy(() => import("@/pages/IndexPage"));
const EntryLayout = lazy(() => import("@/layouts/EntryLayout"));
const NotFound = lazy(() => import("@/components/NotFound"));
const TestPage = lazy(() => import("@/pages/TestPage"));

const fallbackWrapper = (component: React.ReactNode) => {
  return <Suspense fallback={<Spin size='large' />}>{component}</Suspense>;
};

const routes: RouteObject[] = [
  {
    path: "/*",
    element: fallbackWrapper(<EntryLayout />),
    children: [
      {
        path: "",
        element: <Navigate to='/index' />,
      },
      {
        path: "index",
        element: fallbackWrapper(<IndexPage />),
      },
      {
        path: "commit_history",
        element: fallbackWrapper(<CommitPage />),
      },
      {
        path: "new_commit",
        element: fallbackWrapper(<NewCommit />),
      },
      {
        path: "setting",
        element: fallbackWrapper(<ConfigPage />),
      },
      {
        path: "test",
        element: fallbackWrapper(<TestPage />),
      },
      {
        path: "*",
        element: fallbackWrapper(<NotFound />),
      },
    ],
  },
];

const router = createHashRouter(routes);

export default router;
