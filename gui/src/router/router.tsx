import { createHashRouter, Navigate, RouteObject } from 'react-router-dom';
import { lazy, Suspense } from 'react';
import React from 'react';
const Fallback = lazy(() => import('@/components/Fallback'));

const EntryLayout = lazy(() => import('@/layouts/EntryLayout'));
const NotFound = lazy(() => import('@/components/NotFound'));
const ExplorerPage = lazy(() => import('@/pages/ExplorerPage'));
const IndexPage = lazy(() => import('@/pages/ExplorerPage/IndexPage'));
const TestPage = lazy(() => import('@/pages/TestPage'));

const fallbackWrapper = (component: React.ReactNode) => {
  return <Suspense fallback={<Fallback />}>{component}</Suspense>;
};

const routes: RouteObject[] = [
  {
    path: '/*',
    element: fallbackWrapper(<EntryLayout />),
    children: [
      {
        path: '',
        element: <Navigate to='/index' />,
      },
      {
        path: 'index',
        element: fallbackWrapper(<IndexPage />),
      },
      {
        path: 'explorer',
        element: fallbackWrapper(<ExplorerPage />),
      },
      {
        path: 'test',
        element: fallbackWrapper(<TestPage />),
      },
      {
        path: '*',
        element: fallbackWrapper(<NotFound />),
      },
    ],
  },
];

const router = createHashRouter(routes);

export default router;
