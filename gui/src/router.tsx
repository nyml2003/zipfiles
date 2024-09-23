import { createHashRouter, Navigate, RouteObject } from 'react-router-dom';
import EntryLayout from '@/layouts/EntryLayout';
import NotFound from '@/components/NotFound';
import Fallback from '@/components/Fallback';
import { lazy, Suspense } from 'react';
import React from 'react';
const ExplorerPage = lazy(() => import('@/pages/ExplorerPage'));
const IndexPage = lazy(() => import('@/pages/IndexPage'));

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
        path: '*',
        element: <NotFound />,
      },
    ],
  },
];

const router = createHashRouter(routes);

export default router;
